<?php
	define("LOGINSYS_VERSION",		"0.1",TRUE);

	define("LOGINSYS_E_NONE",		0,TRUE);
	
	define("LOGINSYS_E_USER_EXIST",		1,TRUE);	# User already exists
	
	define("LOGINSYS_E_SESSION_EXIST",	1,TRUE);	# Session already exists
	define("LOGINSYS_E_",	,TRUE);
	define("LOGINSYS_E_",	,TRUE);

	define("LOGINSYS_RAND_VIS_NQ",	1,TRUE);
	define("LOGINSYS_RAND_ALNUM",	2,TRUE);
	
	require_once 'CPL/1.0.inc';

	function loginsys_crypt($data)
	{
		$crypt_key = loginsys_conf('crypt_key');
		return crypt($crypt_key,$data);
	}

	function loginsys_conf()
	{
		static $vars	= array();
		$args		= func_get_args();
		$argc		= func_num_args();

		if ($argc == 2)
		{
			$vars[$args[0]] = $args[1];

		} elseif ($argc != 1) {

			loginsys_handle_error("Wrong number of arguments to newsys_conf()");
		}

		if (!array_key_exists($args[0],$vars))
			loginsys_handle_error("Configuration value requested before supplied; key: $args[0]");

		return $vars[$args[0]];
	}

	function loginsys_handle_error($msg)
	{
		CPL::handle_error($msg);
	}

	function loginsys_user_add($dbh,$user)
	{
		$password	= $dbh->prepare_str($user['password'],SQL_REG);

		$dbh->query("	INSERT INTO
					loginsys_users
				(
					password
				) VALUES (
					'$password'
				)");

		return LOGINSYS_E_NONE;
	}

	function loginsys_user_update($dbh,$user)
	{
		$user_id	= (int)$user['user_id'];
		$password	= $dbh->prepare_str($user['password'],SQL_REG);

		$num_users	= $dbh->query("	UPDATE
							loginsys_users
						SET
							password = '$password'
						WHERE
							user_id = $user_id",DB_NULL);

		return $num_users ? LOGINSYS_E_NONE : LOGINSYS_E_USER_EXIST;
	}

	function loginsys_user_remove($dbh,$user_id)
	{
		$user_id 	= (int)$user_id;

		$num_users	= $dbh->query("	DELETE FROM
							loginsys_users
						WHERE
							user_id = $user_id",DB_NULL);

		return $num_users ? LOGINSYS_E_NONE : LOGINSYS_E_USER_EXIST;
	}
	
	function loginsys_auth($dbh,$user)
	{
		$user_id	= (int)$user['user_id'];
		$password	= $dbh->prepare_str($user['password'],SQL_REG);

		$session_id	= $dbh->query("	SELECT
							loginsys_sessions.session_id
						FROM
							loginsys_users,
							loginsys_sessions
						WHERE
							loginsys_users.user_id	= loginsys_session.user_id
						AND	loginsys_users.user_id	= $user_id
						AND	loginsys_users.password	= '$password'",DB_COL)
							
		if ($session_id)
		{
			if (loginsys_session_exists($dbh,$session_id))
				return LOGINSYS_E_NONE;
			else
				return loginsys_session_add($dbh,array('user_id' => $user_id));
		} else {
			# We can't provide any specific error information
			return !LOGINSYS_E_NONE;
		}
	}
	
	function loginsys_session_add($dbh,$session)
	{
		$user_id 	= (int)$session['user_id'];
		$session_key	= loginsys_session_gen_key($dbh);

/*
		# Perhaps we should check here if a session already
		# exists -- or perhaps a configurable option to
		# allow simultaenous logons? Or perhaps clear the
		# old session when creating a new one?

		if
		(
			$dbh->query("	SELECT
						user_id
					FROM
						loginsys_users
					WHERE
						user_id = $user_id",DB_COL)
		)
			return LOGINSYS_E_SESSION_EXISTS;
*/

		$dbh->query("	INSERT INTO
					loginsys_sessions
				(
					user_id,
					session_key,
					act_ts
				) VALUES (
					$user_id,
					'$session_key',
					NOW()
				)",DB_NULL);

		$session_id	= $dbh->last_insert_id();

		return 	array
			(
				'user_id'	=> $user_id,
				'session_id'	=> $session_id,
				'session_key'	=> $session_key
			);
	}

	function loginsys_session_gen_key($dbh)
	{
		$key = "";
		
		do
		{
			$key = loginsys_rand_str(LOGINSYS_KEYSIZE,LOGINSYS_RAND_ALNUM);
			
		} while
		(
			!$dbh->query(sprintf("	SELECT
							user_id
						FROM
							loginsys_sessions
						WHERE
							session_key = '%s'",$dbh->prepare_str($key,SQL_REG),DB_COL))
		);

		return $key;
	}


        function loginsys_rand_str($size,$type = LOGINSYS_RAND_VIS_NQ)
        {
                $random         = "";
                $rand           = "";

                $min            = 0;
                $max            = 0;

                $excluded       = array();

                switch ($type)
                {
                        case LOGINSYS_RAND_VIS_NQ:
                        {
                                $min            = 33;
                                $max            = 126;
                                $excluded       = array(39,34,92); # ' " and \
                                break;
                        }

                        case LOGINSYS_RAND_ALNUM:
                        {
                                $min            = 47;
                                $max            = 122;
                                $excluded       = array(58,59,60,61,62,63,64,91,92,93,94,95,96);
                                break;
                        }

                        default:
                                loginsys_handle_error("Invalid type to newsys_rand_str(); type: $type");
                }

                for ($i = 0; $i < $size; $i++)
                {
                        do
                        {
                                $random = mt_rand($min,$max);

                        } while (in_array($random,$excluded));

                        $rand .= chr($random);
                }

                return $rand;
        }

	function loginsys_session_remove($dbh,$session_id)
	{
		$session_id = (int)$session_id;
		$num_sessions = $dbh->query("	DELETE FROM
							loginsys_sessions
						WHERE
							session_id = $session_id",DB_NULL);

		return $num_sessions ? LOGINSYS_E_NONE : LOGINSYS_E_SESSION_EXIST;
	}

	function loginsys_session_search($dbh,$cri)
	{
		
	}

	function loginsys_session_exists($dbh,$session)
	{
		$dbh->query("	UPDATE
					loginsys_sessions
				SET
					act_ts = NOW()
				WHERE
					session_id = $session_id",DB_NULL);
	}

	function loginsys_sessions($dbh)
	{
		return loginsys_session_search($dbh,array());
	}

	function loginsys_isr_is_valid($table,$field,$op)
	{
		switch ($table)
		{
			case 'loginsys_users':
			{
				break;
			}
			
			case 'loginsys_sessions':
			{
				break;
			}

			default:
				loginsys_handle_error("Invalid table to loginsys_isr_is_valid(); table: $table");
		}
	}
?>
