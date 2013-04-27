<div id="head">
<font size="1"><br /></font>
<font color=0000b2 size="7" face="Verdana"><b><center><a href="index.php">pcf</a></center></b></font>
</div>

<div id="menu">
<ul>
<li><a <?php if($selected=="active") echo "class=\"selected\"";?> href="index.php">Active computers</a></li>
<li><a <?php if($selected=="graphs") echo "class=\"selected\"";?> href="all_graphs.php">All graphs</a>
<ul class="submenu">
<li><a class="submenu <?php if($selected=="graphs" && $type=="tcp") echo "selected";?>" href="all_graphs.php?type=tcp">TCP</a></li>
<li><a class="submenu <?php if($selected=="graphs"&& $type=="icmp") echo "selected";?>" href="all_graphs.php?type=icmp">ICMP</a></li>
<li><a class="submenu <?php if($selected=="graphs"&& $type=="javascript") echo "selected";?>" href="all_graphs.php?type=javascript">Javascript</a></li>
</ul>
</li>
<li><a <?php if($selected=="saved") echo "class=\"selected\"";?> href="saved_computers.php">Saved computers</a></li>
</ul>
</div>
