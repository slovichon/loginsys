<?php
	/*
	 * Loginsys
	 * By Jared Yanovich <jaredy@closeedge.net>
	 * Thursday, March 27, 2003 08:28:12 PM
	 *
	 * crypt.inc - cryptographic routines
	 */

	# loginsys_rand_str() types
	define("LOGINSYS_RAND_VIS_NQ",	1,TRUE);
	define("LOGINSYS_RAND_ALNUM",	2,TRUE);

	function loginsys_crypt($data)
	{
		return crypt(loginsys_conf('crypt_key'), $data);
	}

	function loginsys_rand_str($size,$type = LOGINSYS_RAND_VIS_NQ)
	{
		$random		= "";
		$rand		= "";

		$min		= 0;
		$max		= 0;

		$excluded	= array();

		switch ($type)
		{
			case LOGINSYS_RAND_VIS_NQ:
			{
				$min		= 33;
				$max		= 126;
				$excluded	= array(39,34,92); # ' " and \
				break;
			}

			case LOGINSYS_RAND_ALNUM:
			{
				$min		= 47;
				$max		= 122;
				$excluded	= array(58,59,60,61,62,63,64,91,92,93,94,95,96);
				break;
			}

			default:
				loginsys_handle_error("loginsys_rand_str(): invalid type; type: $type");
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
?>
