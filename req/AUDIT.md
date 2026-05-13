# Minishell Code Audit

## 1. EXEC CLEANING (VALGRIND)

### Current State
The cleanup strategy is split between parent and child processes:

**Child processes (properly exit, cleanup doesn't matter):**
- `do_binary()`: Calls `shell_cleanup(env)` + cleanup of argv/envp/fds before `exit()`
- `do_list_subshell()`: Calls `shell_cleanup(env)` before `exit()`

**Parent process (cleanup MATTERS for valgrind):**
- `do_builtin()`: Cleans up argv and oldfd/fd
- Main loop in `loop()`: After execution, calls `clean_tok(&tok)` and `signo = 0`
- In `execute_buffer()`: Calls `clean_ast(env->ast)` after `do_list()`

### Critical Issues Found

**ISSUE 1a: Double-cleanup / Missing env cleanup in main process**

After a pipeline or group that doesn't fully fork, the environment and export lists may still be modified. However, they're only cleaned up in:
- `shell_cleanup()` - called in CHILD processes only (they exit)
- `shell_exit()` - called when shell exits (EOF)

**The parent process never cleans export/env between commands** except on final exit. This means:
- Commands can accumulate state from `shell_cleanup()` modifications (via `env_import()` in forked children)
- Redirections create temporary file descriptors that might leak on signal interruption
- Between command invocations, the AST is cleaned, but env/export are not

**ISSUE 1b: Redirected file descriptors on signal interruption**

In `redir.c::redir_to_fd()`, files are opened and added to fd array. If a signal arrives after opening but before proper cleanup:
- File descriptors > 2 are stored in `env->duped_fd[]` 
- But if execution is interrupted (signal), these aren't closed until next command or shell exit
- `do_group()` sets `env->duped_fd[0/1] = 1` flags to indicate they should be closed later

The flags are only checked in `shell_cleanup()` which runs in child processes or on exit.

**ISSUE 1c: Potential leak in do_builtin()**

```c
do_builtin(int argc, char **argv, t_env *env, int *fd)
{
    int oldfd[2];
    // ... dup operations ...
    status = do_builtin_match(argc, argv, env);  // calls split_cleanup(argv)
    ft_err(dup2(oldfd[0], 0), "restore error");
    // ...
    split_cleanup(argv);  // <-- SECOND cleanup!
    unset(&oldfd[0]);
    unset(&oldfd[1]);
    unset(&fd[0]);
    unset(&fd[1]);
}
```

`do_builtin_match()` calls `split_cleanup(argv)` inside (argv.c:29), but then `do_builtin()` calls it again. This is a **double-free waiting to happen**.

### Recommendation

1. **Remove the second `split_cleanup(argv)` in do_builtin()** since `do_builtin_match()` already cleans it
2. **Check if env/export should be cleaned between commands** (not just on exit)
3. **Add cleanup of redirected fds in parent process** if signal arrives during execution
4. **Consider refactoring** to ensure `env->duped_fd` flags are checked in parent after signal

---

## 2. SIGNAL HANDLING

### Current State

**In signal.c:**
```c
void	sighands(int n)	//note: sigquit does not stop parsing
{
	if (signo != SIGINT)	//final answer
		signo = n;
}
```

**In exec.c do_list():**
```c
if (signo == SIGINT)
    return (signo);
signo = 0;	//should be sufficient. group and pipe will also check signo.
```

### Issues

**ISSUE 2a: SIGQUIT is treated differently than SIGINT**

- SIGINT is checked immediately in `do_list()` and returns
- SIGQUIT is NOT checked immediately
- The signal handler comment says "sigquit does not stop parsing"
- But throughout the code, `signo != 0` checks exist in heredoc, expansion, etc.

**Current flow:**
1. User hits Ctrl+C → SIGINT → returns immediately from `do_list()`
2. User hits Ctrl+\\ → SIGQUIT → only checked in specific places (heredoc, expansion)
3. If SIGINT arrives while SIGQUIT pending, SIGINT takes precedence and SIGQUIT is lost

**Question: Should SIGQUIT stop execution immediately like SIGINT?**

Bash behavior: Both Ctrl+C (SIGINT) and Ctrl+\\ (SIGQUIT) interrupt and stop execution immediately. Current implementation treats them inconsistently.

**ISSUE 2b: SIGQUIT can be lost**

In `sighands()`:
```c
if (signo != SIGINT)	//final answer
    signo = n;
```

If SIGINT is pending and then SIGQUIT arrives, the SIGQUIT is ignored. If then the signal is cleared (`signo = 0` in loop), SIGQUIT is lost.

**ISSUE 2c: Signal reset in main loop**

```c
signo = 0;  // In loop(), after each command
```

This happens unconditionally, even if a signal arrived. Better to only reset if no signal is pending, or handle signals more carefully.

### Recommendation

1. **Decide on SIGQUIT behavior**: Should it stop execution immediately or only in certain contexts?
   - Current code suggests it's meant to be immediate (based on checks throughout)
   - Change `do_list()` to also check for SIGQUIT immediately, or
   - Remove the special "does not stop parsing" behavior

2. **Fix signal precedence logic** to not lose signals:
   ```c
   void sighands(int n)
   {
       signo = n;  // Always update, latest signal takes precedence
   }
   ```
   Or:
   ```c
   void sighands(int n)
   {
       if (!signo)  // Only update if no signal pending yet
           signo = n;
   }
   ```

3. **Check both signals immediately in do_list()**:
   ```c
   if (signo == SIGINT || signo == SIGQUIT)
       return (signo);
   ```

---

## 3. FUNCTION CALLING DO_LIST (ARCHITECTURE DECISION)

### Current Call Chain

```
main()
  └─ loop()
       └─ execute_buffer()
            └─ do_list()
```

### Current execute_buffer() Implementation

```c
int execute_buffer(t_env *env, t_tok **tok)
{
    env->duped_fd[0] = 0;      // Reset before each command
    env->duped_fd[1] = 0;
    env->do_not_subshell = 0;
    make_word(*tok);
    env->ast = parse(tok);
    
    if (env->ast)
        do_list(env->ast, env);
    
    clean_ast(env->ast);       // Clean AST
    env->ast = NULL;
    
    return (env->last);
}
```

Then back in `loop()`:
```c
clean_tok(&tok);  // Clean tokens
signo = 0;        // Reset signal
```

### Issues

**ISSUE 3a: Unclear ownership of cleanup**

- `env->ast` is cleaned in `execute_buffer()`
- But `shell_cleanup(env)` (called in child processes) also calls `clean_ast()`
- The pattern is: parent cleans AST, children clean AST+env+export+pipes

**ISSUE 3b: Reset of duped_fd before execution**

The reset of `env->duped_fd[0/1]` at the start of `execute_buffer()` makes sense for a new command, but:
- These flags indicate if stdin/stdout were redirected in a subshell
- They're only meaningful within one execution
- But if a command leaves them as 1, and then we return to `execute_buffer()`, they're immediately reset

This is probably fine, but the comment says "should not be needed in main process", suggesting uncertainty about the design.

**ISSUE 3c: Signals can cause premature return**

If a signal arrives during parsing or AST building (before `do_list()`), the AST might be partially built but `signo != 0`. Then:
- `do_list()` returns immediately
- AST is cleaned
- Back to main loop

This is correct behavior, but depends on knowing that the AST is safe to clean at any point.

### Recommendation

**Current structure is reasonable, but document the invariants:**

1. **In execute_buffer():**
   ```c
   // Reset execution state for new command
   env->duped_fd[0] = 0;
   env->duped_fd[1] = 0;
   env->do_not_subshell = 0;
   
   // Parse command into AST
   make_word(*tok);
   env->ast = parse(tok);
   
   // Execute (or return early if signal during parsing)
   if (env->ast)
       do_list(env->ast, env);
   
   // Cleanup this execution's resources
   // Note: env/export are NOT cleaned here - they persist across commands
   //       They are only cleaned on shell exit (shell_exit) or on fork cleanup (shell_cleanup)
   clean_ast(env->ast);
   env->ast = NULL;
   
   return (env->last);
   ```

2. **Consider if env/export should be cleaned between commands** - currently they accumulate state only between shell restarts

3. **Alternative architecture** (if unhappy with current design):
   - Move `clean_tok(&tok)` and `signo = 0` into `execute_buffer()`
   - Make `execute_buffer()` completely responsible for its lifecycle
   - But this would require changing the loop structure

---

## Summary of Action Items

| Issue | Severity | Action |
|-------|----------|--------|
| 1a: Double cleanup in do_builtin | HIGH | Remove second `split_cleanup(argv)` call |
| 1b: Leaked redir fds on signal | MEDIUM | Add signal-safe cleanup of open fds in parent |
| 1c: Double-free risk | HIGH | Fix double cleanup |
| 2a: SIGQUIT inconsistency | MEDIUM | Decide if SIGQUIT should stop immediately |
| 2b: Signal loss | MEDIUM | Fix signal precedence logic |
| 2c: Signal reset logic | LOW | Document or refine when signo is reset |
| 3a: Cleanup ownership | LOW | Document the cleanup contract |
| 3b: duped_fd reset | LOW | Document the reset as expected behavior |
| 3c: Signal during parse | LOW | Verify AST is always safe to clean |
