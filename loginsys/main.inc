<?php
	/*
	 * Loginsys
	 * By Jared Yanovich <jaredy@closeedge.net>
	 * Monday, March 17, 2003 10:25:06 AM
	 *
	 * main.inc - main routines/definitions
	 */

	define("LOGINSYS_VERSION",	"-current",TRUE);
	define("E_LOGINSYS_NONE",	0,TRUE);

	function loginsys_conf()
	{
		static $vars	= array();
		$args		= func_get_args();
		$argc		= func_num_args();

		if ($argc == 2)
		{
			$vars[$args[0]] = $args[1];

		} elseif ($argc != 1) {

			loginsys_handle_error("loginsys_conf(): bad arguments");
		}

		if (!array_key_exists($args[0],$vars))
			loginsys_handle_error("loginsys_conf(): configuration value requested before supplied; key: $args[0]");

		return $vars[$args[0]];
	}

	require_once 'CPL/1.0.inc';
	require_once 'loginsys-config.inc';

	$loginsys_sys_root = loginsys_conf('sys_root');

	require_once "$loginsys_sys_root/misc.inc";
	require_once "$loginsys_sys_root/session.inc";
	require_once "$loginsys_sys_root/user.inc";
?>
