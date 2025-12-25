--TEST--
Check if pdotrace php.ini loaded
--EXTENSIONS--
pdotrace
--FILE--
<?php
	pdotrace_log_file_path();
?>
--EXPECT--
/extensions/tmp/pdotrace.log
