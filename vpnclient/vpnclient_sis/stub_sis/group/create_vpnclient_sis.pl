#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Updates pkg-files' and vpnclient_sis.mk's version information with current date.
# Creates temporary pkg-files with updated version.
#

use strict;
use POSIX qw(strftime);
use File::Copy;


#Subroutines to update version information
my $PLATFORM = "42";

#
# Creates a temp file and replaces line
#
sub replace_line_and_create_temp
{
	# arguments
	my ($pattern, $replacement, $path) = @_;
		
	# create temp file
	my $temp_path = "temp_".$path;
	if (-e $temp_path)
	{
		chmod 0666, $temp_path;
		unlink $temp_path;
	}
	copy($path, $temp_path);
	chmod 0666, $temp_path;
	
	# update version information
	my ($infile, $outfile);
	open $infile, "<".$path or die "Can't open $path : $!";
	open $outfile, ">".$temp_path or die "Can't open $temp_path : $!";
	while (<$infile>)
	{
		my $line = $_;
		if ($line =~ s/$pattern/$replacement/g)
		{
			print "Replaced line in $temp_path\n";
		}
		print $outfile $line;
	}
	close $infile;
	close $outfile;
}


# Define variables
my $pkgtime = strftime($PLATFORM.",%y,%m%d", localtime());
my $mktime = strftime("%y%m%d", localtime());
my $epocroot = $ENV{'EPOCROOT'};
$epocroot =~ s/\\/\//;

my $rdderdir="../data/RDTest_02.der";
my $rdkeydir="../data/RDTest_02.key";
my $localisation_files = $epocroot . "epoc32/data/Z/resource/vpnecomnotifier.r05";

if (!(-e $localisation_files))
{
 	die "Localisation files are not available! Exiting..\n";
} 


my $numArgs = $#ARGV + 1;
if (!($#ARGV + 1 eq 1))
{
	die "wrong number of command line arguments.";
}
my $config = @ARGV[0];

# Creates temp pkg files with updated version information
if ($config eq "UREL")
{

# stub_nokia_vpn_vpnpolins_armv5.pkg
replace_line_and_create_temp(
    '^#{"Nokia VPN Policy Installer Stub"},\(0xA0000131\),.*?,.*?,.*?',
    "#{\"Nokia VPN Policy Installer Stub\"},(0xA0000131),$pkgtime",
    'stub_vpnpolins_armv5.pkg');
    
# stub_nokia_vpn_client_localised_armv5.pkg
replace_line_and_create_temp(
    '^\(0x101F5147\),.*?,.*?,.*?$',
    "(0x101F5147),$pkgtime",
    'stub_nokia_vpn_client_armv5.pkg');
	
	print "Processing stub_vpnpolins_armv5.pkg\n";
	system("makesis -v -s stub_vpnpolins_armv5.pkg ipsecvpn_vpnpolins.SIS") == 0 or 
		die  "ipsecvpn_vpnpolins.SIS creation failed";
	
	
	print "Processing stub_nokia_vpn_client_armv5.pkg\n";
	system("makesis -v -s stub_nokia_vpn_client_armv5.pkg ipsecvpn.SIS") == 0 or 
		die "ipsecvpn.SIS creation failed";
	
	print "Export stub sis files to " . $epocroot . "epoc32/data/z/system/install\n";
	copy("ipsecvpn_vpnpolins.SIS", $epocroot . "epoc32/data/z/system/install") or die "ipsecvpn_vpnpolins.SIS cannot be copied.";
	copy("ipsecvpn.SIS", $epocroot . "epoc32/data/z/system/install") or die "ipsecvpn_vpnpolins.SIS cannot be copied.";
	
	# remove temp files
	unlink <temp_*.pkg>;  
		
}
elsif ($config eq "UDEB")  
{
  #do nothing
}
else
{
	die "Unknown config " . $config . "\n";
}
		    