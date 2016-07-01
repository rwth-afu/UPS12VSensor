#!/usr/bin/perl -w

my $server_ip = "127.0.0.1";
my $server_port = "1234";

if ($ARGV[0] and $ARGV[0] eq "config") {
	print "graph_title Voltage and Current on 12 V UPS\n";
	print "graph_args --base 1000 --lower-limit 0\n";
	print "graph_vlabel Value in Volt or Ampere\n";
	print "graph_category ups\n";
	print "graph_info This graph shows the voltage and current on the 12 V UPS board.\n";
	print "UBat.label Voltage Bat\n";
	print "UBat.type GAUGE\n";
	print "UNT.label Voltage Power Supply\n";
	print "UNT.type GAUGE\n";
	print "IBat.label Current Bat\n\n";
	print "IBat.type GAUGE\n";
	print "INT.label Current Power Supply\n";
	print "INT.type GAUGE\n";
else {

// Open ServerPort

// Read Values

// Give out on STDOUT in format
//  UBat.value 12.234
//  IBat.value 1.234
//  UNT.value 12.234
//  INT.value 1.234
}
