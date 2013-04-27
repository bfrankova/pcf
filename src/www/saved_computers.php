<!doctype html public "-//W3C//DTD HTML 4.0 //EN">

<html>
<head>
	<title>pcf</title>
	<script type="text/javascript" src="js.js"></script>
	<style type="text/css">
		@import "style.css";
	</style>
</head>

<body onload="loadCookies2()">

	
<?php 
$selected="saved";
include "header.php"; 
?>

<div id="content">
<div id="aktual">

<?php

$database = "data/database.xml";

if ($_POST["skew"]) {
	$xmlDoc = new DOMDocument();
	$xmlDoc->load($database);
	$xpath = new DOMXpath($xmlDoc);
	foreach($xpath->query("//computer[@skew=\"" . $_POST['skew'] . "\"]") as $computer) {
		$dom = dom_import_simplexml($computer);
		$s = simplexml_import_dom($dom);
		if ($s->name == $_POST["name"]) {
			$dom->parentNode->removeChild($dom);
		}
	}

	$xmlDoc->save($database);
}

if (file_exists($database)) {
        $computers = simplexml_load_file($database);
}
else {
        #exit("Failed to open $filename");
        exit();
}

echo "<span style='position: absolute; top: 120px; right: 10px'><a href='javascript:fold(200)'>[+/-]</a></span>";

$i = 200;
foreach ($computers->computer as $computer) {
	echo "<b><font color='#0000b2'><a href=\"javascript:aktual('", $i, "')\">", $computer->name, "</font></a></b><br />";
	echo "<form method='post' action='", $_SERVER['PHP_SELF'], "'>";
	echo "<table id='",$i, "'>";
	echo "<tr><td width='40%'>Skew:</td><td>", $computer["skew"], "</td></tr>";
	echo "<tr><td>Address:</td><td>", $computer->address, "</td></tr>";
	echo "<tr><td>Date:</td><td>", $computer->date, "</td></tr>";

	echo "<tr><td></td><td><input type='submit' name='delete' value='Delete computer' /></td></tr>";
	echo "<input type='hidden' name='skew' value='", $computer['skew'], "' />";
	echo "<input type='hidden' name='name' value='", $computer->name, "' />";

	#echo "<script>aktual('", $i, "')</script>";
	$i = $i + 1;
	echo "</table>";
	echo "</form>";
}

?>

</div>
</div>

</body>
</html>
