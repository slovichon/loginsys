<?php
	/*
	 * Loginsys
	 * By Jared Yanovich <jaredy@closeedge.net>
	 * Monday, March 17, 2003 10:31:12 AM
	 *
	 * session.inc - session routines
	 */

	define("LOGINSYS_MULTI_DENY",	0,TRUE);
	define("LOGINSYS_MULTI_ALLOW",	1,TRUE);
	define("LOGINSYS_MULTI_CLEAR",	2,TRUE);

	define("E_LOGINSYS_SESSION_EXIST",	1,TRUE);	# Session already exists

	function loginsys_auth($dbh,$user)
	{
		$user_id	= (int)$user['user_id'];
		$password	= $dbh->prepare_str($user['password'],SQL_REG);

		$session_id	= $dbh->query("	SELECT
							s.session_id
						FROM
							loginsys_users		u,
							loginsys_sessions	s
						WHERE
							u.user_id	= s.user_id
						AND	u.user_id	= $user_id
						AND	u.password	= '$password'",DB_COL)

		if ($session_id)
		{
			if (loginsys_session_exists($dbh,$session_id))
				return E_LOGINSYS_NONE;
			else
				return loginsys_session_add($dbh,array('user_id' => $user_id));
		} else {
			# We can't provide any specific error information
			return !E_LOGINSYS_NONE;
		}
	}

	function loginsys_session_add($dbh,$session)
	{
		$user_id 	= (int)$session['user_id'];

		# Perhaps we should check here if a session already
		# exists -- or perhaps a configurable option to
		# allow simultaenous logons? Or perhaps clear the
		# old session when creating a new one?

		if
		(
			loginsys_conf("no_simult") &&
			$dbh->query("	SELECT
						user_id
					FROM
						loginsys_sessions
					WHERE
						user_id = $user_id",DB_COL)
		)
			return E_LOGINSYS_SESSION_EXISTS;

		$session_key	= loginsys_session_gen_key($dbh);

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

	function loginsys_session_remove($dbh,$session_id)
	{
		$session_id = (int)$session_id;
		$num_sessions = $dbh->query("	DELETE FROM
							loginsys_sessions
						WHERE
							session_id = $session_id",DB_NULL);

		return $num_sessions ? E_LOGINSYS_NONE : E_LOGINSYS_SESSION_EXIST;
	}

	function loginsys_session_search($dbh,$cri)
	{
		
	}

	function loginsys_session_exists($dbh,$session_id)
	{
		$session_id = (int)$session_id;

		return $dbh->query("	UPDATE
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
?>
