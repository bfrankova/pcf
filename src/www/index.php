<!doctype html public "-//W3C//DTD HTML 4.0 //EN">

<html>
<head>
	<title>pcf</title>
	<script type="text/javascript" src="js.js"></script>
	<style type="text/css">
		@import "style.css";
	</style>
</head>

<body onload="loadCookies1()">
	
<?php 
$selected="active";
include "header.php";
?>

<div id="content">
<div id="aktual">

<?php

error_reporting(E_ERROR | E_WARNING | E_PARSE);

function debugOutput($message)
{
	echo "<div style=\"color: red; background-color: yellow; \">$message</div>";
}

if (isset($_POST["name"])) {
	
	/* save routine START */
	$database = "data/database.xml";
	
	if (!file_exists($database)) {
		$xmlDoc = new DOMDocument();
		$root = $xmlDoc->appendChild($xmlDoc->createElement("computers"));
		$xmlDoc->formatOutput = true;
		$xmlDoc->save($database);
	}
	$computers = simplexml_load_file($database);
	$computer = $computers->addChild("computer");
	$computer->addAttribute("skew", $_POST["skew_tcp"]);

	$computer->addChild("name", $_POST["name"]);

	$computer->addChild("address", $_POST["address_tcp"]);

	$computer->addChild("frequency", $_POST["frequency_tcp"]);

	$computer->addChild("date", $_POST["date_tcp"]);

	$computers->asXML($database);
	/* save routine END */
	
}

if (file_exists("data/active.xml")) {
	$computersTcp = simplexml_load_file("data/active.xml");
}
if (file_exists("data/javascript/active.xml")){
	$computersJavascript = simplexml_load_file("data/javascript/active.xml");
}
if (file_exists("data/icmp/active.xml")){
	$computersIcmp = simplexml_load_file("data/icmp/active.xml");
}

function printComputers($computers){
	echo "<hr>";
	foreach ($computers->computer as $computer){
		echo "address: " . $computer->address . "<br/>";
	}
}

/*
echo printComputers($computersTcp);
echo printComputers($computersIcmp);
echo printComputers($computersJavascript);
*/

$mergedComputers = array();

class MergedComputerInfo{
	function __construct($address){
		$this->address = $address;
	}
	public $address;
	public $name;
	public $skews = array();
	public $dates = array();
	public $packets = array();
	public $frequencies = array();
}

function addComputer($computer, $type){
	global $mergedComputers;
	
	$addressString = (string) $computer->address;
	if(!array_key_exists($addressString, $mergedComputers)){
		$mergedComputers[$addressString] = new MergedComputerInfo($addressString);
		$mergedComputers[$addressString]->address = $addressString;
		$mergedComputers[$addressString]->name = (string)$computer->name;
	}
	
	$mergedComputers[$addressString]->skews[$type] = (string)$computer["skew"];
	$mergedComputers[$addressString]->dates[$type] = (string)$computer->date;
	$mergedComputers[$addressString]->packets[$type] = (string)$computer->packets;
	$mergedComputers[$addressString]->frequencies[$type] = (string)$computer->frequency;
}

foreach ($computersTcp->computer as $computer){
	addComputer($computer, "tcp");
}

foreach ($computersJavascript->computer as $computer){
	addComputer($computer, "javascript");
}

foreach ($computersIcmp->computer as $computer){
	addComputer($computer, "icmp");
}

// echo serialize($mergedComputers);


echo "<span style='position: absolute; top: 120px; right: 10px'><a href='javascript:fold(100)'>[+/-]</a></span>";

$i = 100;
/*
foreach ($computers->computer as $computer) {
	if (isset($computer->identity)) {
		echo "<b><font color='green'><a href=\"javascript:aktual('", $i, "')\">", $computer->address, "</a></font></b><br />";
        	echo "<div id='", $i, "'>";
	        echo "<form method='post' action='", $_SERVER['PHP_SELF'], "'>";
	        echo "<table>";
	        echo "<tr><td width='40%'>Skew:</td><td>", $computer["skew"], "</td></tr>";
		echo "<tr><td>Packets:</td><td>", $computer->packets, "</td></tr>";
	        foreach ($computer->identity as $identity) {
	               echo "<tr><td>Similar skew:</td><td>", $identity->name, "</td></tr>";
	               echo "<tr><td>Diff:</td><td>", $identity->diff, "</td></tr>";
	        }
	        echo "<tr><td>Date:</td><td name='date'>", $computer->date, "</td></tr>";
	        echo "<tr><td>Name:</td><td><input type='text' name='name' size='31' /></td><td><input type='submit' value='Save computer'></td></tr>";

	        echo "<input type='hidden' name='skew' value='", $computer["skew"], "' />";
	        echo "<input type='hidden' name='address' value='", $computer->address, "' />";
	        echo "<input type='hidden' name='frequency' value='", $computer->frequency, "' />";
	        echo "<input type='hidden' name='date' value='", $computer->date, "' />";

        #echo "<script>aktual('", $i, "')</script>";
	        echo "</table>";
	        echo "</form>";

        	$i = $i + 1;
		echo "<font color='#0000b2'><a href=\"javascript:aktual('", $i, "')\">Show graph</a></font><br /><br />";
	        echo "<center><img id='", $i, "' src='graph/", $computer->address, ".svg' /></center>";

	        echo "</div>";

	        $i = $i + 1;
	}
}
*/

echo "<br />";


foreach ($mergedComputers as $computer) {
	if (!isset($computer->identity)) {
		echo "<b><font color='#0000b2'><a href=\"javascript:aktual('", $i, "')\">", $computer->address, "</a></font></b><br />";
		echo "<div id='", $i, "'>";
		
		echo "<table border='1'>";
		if ($computer->name != "") {
			echo "<tr><td>Diff:</td><td>", $computer->diff, "</td></tr>";
		}
		echo "<tr><th>&nbsp;</th><th>TCP</th><th>Javascript</th><th>ICMP</th></tr>";
		echo "<tr><td width='150px'>Skew:</td><td width='250px'>" . $computer->skews["tcp"] . "</td><td width='250px'>" . $computer->skews["javascript"] . "</td><td width='250px'>" . $computer->skews["icmp"] . "</td></tr>";
		echo "<tr><td>Packets:</td><td>" . $computer->packets["tcp"] . "</td><td>" . $computer->packets["javascript"] . "</td><td>" . $computer->packets["icmp"] . "</td></tr>";
		echo "<tr><td>Date:</td><td>" . $computer->dates["tcp"] . "</td><td>" . $computer->dates["javascript"] . "</td><td>" . $computer->dates["icmp"] . "</td></tr>";
		echo "</table>";
		echo "</br>";
	
		echo "<form method='post' action='", $_SERVER['PHP_SELF'], "'>";
		echo "<span style='margin-right: 20px;'>Name:</span><input type='text' name='name' size='45' /><input type='submit' value='Save computer'>";
	
		/* triplication START */
		echo "<input type='hidden' name='skew_tcp' value='", $computer->skews["tcp"], "' />";
		echo "<input type='hidden' name='address_tcp' value='", $computer->address, "' />";
		echo "<input type='hidden' name='frequency_tcp' value='", $computer->frequencies["tcp"], "' />";
		echo "<input type='hidden' name='date_tcp' value='", $computer->dates["tcp"], "' />";
		/* triplication END */

		#echo "<script>aktual('", $i, "')</script>";
		
		echo "</form>";
		echo "</br>";

		$i = $i + 1;
		echo "<font color='#0000b2'><a href=\"javascript:aktual('", $i, "')\">Show graph</a></font><br /><br />";
		echo "<center>";
		echo "<img id='", $i, "' src='graph/", $computer->address, ".svg' />";
		echo "<img id='", $i, "' src='graph/javascript/", $computer->address, ".svg' />";
		echo "<img id='", $i, "' src='graph/icmp/", $computer->address, ".svg' />";
		echo "</center>";

		echo "</div>";

		$i = $i + 1;
	}
}

?>

</div>
</div>

</body>
</html>
