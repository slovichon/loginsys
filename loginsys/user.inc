<?php
	/*
	 * Loginsys
	 * By Jared Yanovich <jaredy@closeedge.net>
	 * Monday, March 17, 2003 10:33:03 AM
	 *
	 * user.inc - user routines
	 */

	define("E_LOGINSYS_USER_EXIST",		1,TRUE);	# User already exists

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

		return E_LOGINSYS_NONE;
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

		return $num_users ? E_LOGINSYS_NONE : E_LOGINSYS_USER_EXIST;
	}

	function loginsys_user_remove($dbh,$user_id)
	{
		$user_id 	= (int)$user_id;

		$num_users	= $dbh->query("	DELETE FROM
							loginsys_users
						WHERE
							user_id = $user_id",DB_NULL);

		return $num_users ? E_LOGINSYS_NONE : E_LOGINSYS_USER_EXIST;
	}
?>
