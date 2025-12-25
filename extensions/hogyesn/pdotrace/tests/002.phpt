--TEST--
Check if pdotrace php.ini loaded
--EXTENSIONS--
pdotrace
--INI--
pdotrace.enabled=1
pdotrace.log_file_path=/extensions/tmp/pdotrace.log
--FILE--
<?php
	pdotrace_log_file_path();
	echo ini_get('pdotrace.log_file_path') . PHP_EOL;
	var_dump(ini_get('pdotrace.enabled'));
?>
--EXPECT--
/extensions/tmp/pdotrace.log
/extensions/tmp/pdotrace.log
string(1) "1"
