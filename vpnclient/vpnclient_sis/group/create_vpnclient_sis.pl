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
my $PLATFORM = "50";

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
# nokia_vpn_client_localised_armv5_urel.pkg
	replace_line_and_create_temp(
	    '^\(0x101F5147\),.*?,.*?,.*?, TYPE=SA, RU$',
	    "(0x101F5147),$pkgtime, TYPE=SA, RU",
	    'nokia_vpn_client_localised_armv5_urel.pkg');

	print "Processing temp_nokia_vpn_client_localised_armv5_urel.pkg\n";
	system("makesis -v temp_nokia_vpn_client_localised_armv5_urel.pkg mVPN_RnD_" . $PLATFORM . "_" . $mktime . "_urel.sis") == 0 or 
		die "UREL sis creation failed";  		
	print "\nSigning created mVPN_RnD_" . $PLATFORM . "_" . $mktime . "_urel.sis\n";
	system("signsis mVPN_RnD_" . $PLATFORM . "_" . $mktime . "_urel.sis mVPN_RnD_" . $PLATFORM . "_" . $mktime . "_urel.sis " . $rdderdir . " " . $rdkeydir) == 0 or 
		die "UREL sis signing failed";

	# remove temp files
	unlink <temp_*_urel.pkg>;  

	# nokia_vpn_client_localised_armv5_udeb.pkg
		replace_line_and_create_temp(
		    '^\(0x101F5147\),.*?,.*?,.*?, TYPE=SA, RU$',
		    "(0x101F5147),$pkgtime, TYPE=SA, RU",
		    'nokia_vpn_client_localised_armv5_udeb.pkg');
		
		print "Processing temp_nokia_vpn_client_localised_armv5_udeb.pkg\n";
		my $udeb_sis_file_name = "mVPN_RnD_" . $PLATFORM . "_" . $mktime . "_udeb.sis";
		system("makesis -v temp_nokia_vpn_client_localised_armv5_udeb.pkg " . $udeb_sis_file_name ) == 0 or 
			die "UDEB sis creation failed";			
		print "\nSigning created mVPN_RnD_" . $PLATFORM . "_" . $mktime . "_udeb.sis\n";
		system ("signsis " . $udeb_sis_file_name . " " . $udeb_sis_file_name . " "  . $rdderdir . " " . $rdkeydir)  == 0 or 
			die "UDEB sis signing failed";	
			
	# remove temp files
	unlink <temp_*_udeb.pkg>;
			
}
elsif ($config eq "UDEB")  
{
#empty
}
else
{
	die "Unknown config " . $config . "\n";
}
		    