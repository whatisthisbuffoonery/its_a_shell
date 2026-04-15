int	cleanup(int code, t_cmd **cmd, t_env *env)
{
	static t_cmd	**cmd_list;
	static t_env	*env_lists;

	if (!cmd_list)
	{
		cmd_list = cmd;
		env_lists = env;
		return (0);
	}
	//clean all lists
	exit(code);
	return (0);
}
