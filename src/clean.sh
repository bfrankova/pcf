
cp www/data/database.xml .
mv www/data/icmp/database.xml databaseIcmp.xml
mv www/data/javascript/database.xml databaseJavascript.xml

rm -rf www/graph/*
rm -rf www/data/*
rm -rf graph/*
rm -rf log/*

mkdir www/graph/icmp
mkdir www/data/icmp
mkdir graph/icmp
mkdir log/icmp 

mkdir graph/javascript log/javascript www/graph/javascript www/data/javascript

mv database.xml www/data/
mv databaseIcmp.xml www/data/icmp/database.xml
mv databaseJavascript.xml www/data/javascript/database.xml
