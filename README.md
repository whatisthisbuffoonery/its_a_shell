*This project has been created as part of the 42 curriculum by dthoo and achew*  
  
## Description  
  
Minishell is an emulation of a small subset of Bash. This subset includes:  
	env variables, not including positional parameters  
	basic working history, not including history expansion  
	execution of binaries, either directly or through PATH  
	echo, pwd, cd, env, export, unset, and exit as builtins  
	handling for SIGINT, SIGQUIT, and ctrl d  
	checking exit status through $?  
	single and double quote handling, except for unclosed quotes  
	redirections: >, >>, <, <<  
	pipelines  

