--TEST--
Check if pdotrace php.ini loaded
--EXTENSIONS--
pdotrace
--FILE--
<?php
	pdotrace_log_file_path();
	echo ini_get('pdotrace.log_file_path');
?>
--EXPECT--
/extensions/tmp/pdotrace.log
/extensions/tmp/pdotrace.log
