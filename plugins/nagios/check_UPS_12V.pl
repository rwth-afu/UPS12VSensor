#!/usr/bin/perl -w
#
# Check values of voltages and currents from 12 V UPS Board

use strict;
use warnings;
use Getopt::Std;
use IO::Socket::INET;

my $server_host = "localhost";
my $server_port = "223";

my(%ERRORS) = ( OK=>0, WARNING=>1, CRITICAL=>2, UNKNOWN=>3 );

my $status = $ERRORS{OK};
my $message;

my $debug_flag=0;

our($opt_c, $opt_w, $opt_W, $opt_C, $opt_h, $opt_d, $opt_s);

getopts("w:W:c:C:hd:s:");

sub printhelp () {
        print "Usage: check_UPS_12V [-h] -d [U|I] -s [BAT|SUPPLY] -c lowcritical -w lowwarning -W highwarning -C highcritical\n";
        print "-h Help, this text\n";
		print "-d U for Voltage, I for Current\n";
		print "-s BAT for battery branch, SUPPLY for stationary power supply\n";
		print "-c num Critical threshold for low value, just for voltages\n";
		print "-w num Warning threshold for low value, just for voltages\n";
		print "-W num Warning threshold for high value, both current and voltages\n";
		print "-C num Critical threshold for high vlaue, both current and voltages\n";
        
        print "\n\t\tby Ralf Wilke rwth-afu\@online.de\n\t\thttp://www.afu.rwth-aachen.de\n";
        if ($debug_flag) {
                print "opt_c:$opt_c opt_w:$opt_w opt_W:$opt_W opt_C:$opt_C opt_h:$opt_h opt_d:$opt_d opt_s:$opt_s\n";
        }

        exit $status;
} 

#sanity check
if (!defined $opt_c||!defined $opt_w||!defined $opt_W||!defined $opt_C||!defined $opt_d||!defined $opt_s||$opt_h) {
        $status= $ERRORS{UNKNOWN};
        &printhelp;
} elsif ($opt_c > $opt_w) {
        print "Critical low threshold must be higher or equal to warning low threshold\n";
        $status= $ERRORS{UNKNOWN};
        &printhelp;
} elsif ($opt_w > $opt_W || $opt_c > $opt_C) {
        print "Lower tresholds must be lower then higher thresholds\n";
        $status= $ERRORS{UNKNOWN};
        &printhelp;
} elsif ($opt_C < $opt_W) {
        print "Higher critical threshold must be higher or equal to higher warning threshold\n";
        $status= $ERRORS{UNKNOWN};
        &printhelp;
} elsif (not($opt_d eq "U"||$opt_d eq "I")) {
        print "Dimension must be either U or I for voltage or Current\n";
        $status= $ERRORS{UNKNOWN};
        &printhelp;
} elsif (not($opt_s eq "BAT"||$opt_d eq "SUPPLY")) {
        print "Source must be either BAT or SUPPLY for battery or stationary power supply\n";
        $status= $ERRORS{UNKNOWN};
        &printhelp;
}



# Open ServerPort
# flush after every write
$| = 1;

my $socket;

# creating object interface of IO::Socket::INET modules which internally creates 
# socket, binds and connects to the TCP server running on the specific port.
unless ($socket = new IO::Socket::INET (
	PeerHost => $server_host,
	PeerPort => $server_port,
	Proto => 'tcp',
	)) {
		$status = $ERRORS{CRITICAL};
		$message = "UNKNOWN\|Data Server not running at $server_host:$server_port\n";
		print "$message\n";
		exit $status;
}

if ($debug_flag) {
	print "TCP Connection Success.\n";
}


# read the socket data sent by server.
my $data = <$socket>;

print "Received from Server : $data\n";

# Todo: Parse data received from server
# Ausserdem Format-Line verwerfen

# Extract data separated by ";"
my ($ubat,$ibat,$unt,$int) = split /;/, $data;



#$value =~ s/^\s*(.*?)\s*$/$1/; #remove whitespaces
#unless (($value =~ /^-?(?:\d+(?:\.\d*)?|\.\d+)$/) || ($value =~ /^[+-]?\d+$/)) #check that it is an integer or decimal returned
#{
#        $message="Did not got an integer or a decimal from temperature probe";
#        $status=$ERRORS{CRITICAL};
#}
#$value = sprintf("%.2f", $value);

# TODO: Je nach gewaehlter Option die Grenzwertbetrachung machen.


if ($debug_flag) {
	print "opt_c:$opt_c opt_w:$opt_w opt_W:$opt_W opt_C:$opt_C opt_h:$opt_h opt_d:$opt_d opt_s:$opt_s\n";
}



if ($value <= $opt_c) {
        $status=$ERRORS{CRITICAL};
        $message="CRITICAL";
} elsif ($value > $opt_c && $value <= $opt_w) {
        $status=$ERRORS{WARNING};
        $message="WARNING";
} elsif ($value >  $opt_w && $value <  $opt_W) {
        $status=$ERRORS{OK};
        $message="OK";
} elsif ($value >= $opt_W && $value < $opt_C) {
        $status=$ERRORS{WARNING};
        $message="WARNING";
} elsif ($value >= $opt_C) {
        $status=$ERRORS{CRITICAL};
        $message="CRITICAL";
} else { #This should never happend
        $status=$ERRORS{UNKNOWN};
        $message="UNKNOWN";
}

print "$message: $value C\|temperature=$value;$opt_c;$opt_w;$opt_W;$opt_C\n";
exit $status;

