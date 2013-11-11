rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of the License "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description: 
rem

@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S %0 %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
if errorlevel 1 goto script_failed_so_exit_with_non_zero_val 2>nul
goto endofperl
@rem ';
#!perl
#line 15

use strict;
use warnings;
use FindBin qw($Bin);
use Getopt::Long;
use File::Find;
use File::Path;
use Cwd;
use XML::Simple;
use Archive::Zip;
use Archive::Zip::Tree;


my $path = ".";
my $dir = substr( $Bin, 0, 2 ) . "/epoc32/data/";
my $Widgetdir = $dir . "WidgetPreInstall";
mkdir $dir;


 print "\n\nScanning directory for .wgz files...\n\n";
 my $workdir;
 my $zip = Archive::Zip->new();
 my $status;


 opendir(DIR,$path);
 
 while ($workdir=readdir(DIR))
 {
 if($workdir =~ /wgz/)
 {
    print "extracting $workdir...\n";
    $status = $zip->read($workdir);    
    $zip->extractTree( '', $path );    # extract files
  }
  }
  closedir(DIR);
  print "\n\n";




print "\n\nParsing Info.plist and moving to WidgetPreInstall directory...\n\n";
my $xml = new XML::Simple;
rmtree($Widgetdir, 0, 0);
mkdir $Widgetdir;
opendir(DIR,$path);
 while ( my $entry = readdir( DIR ) ) 
 {
    if (( -d "$path\\$entry" ) && ($entry ne '.' && $entry ne '..' && $entry ne '.svn') )
      {
        print "Parsing $entry\\Info.plist ...\n";
	my $data = $xml->XMLin("$path\\$entry\\Info.plist");

	my $i = -1;
	my $ident;

	foreach my $key (@{$data->{dict}->{key}}) {
		    $i = $i + 1;
	    if($key eq 'Identifier') {
	      $ident = $data->{dict}->{string}->[$i];
	      last;
	    }
	  }
	mkdir "$Widgetdir\\$ident";
        rename "$entry", "$Widgetdir\\$ident\\$entry";
        print "$Widgetdir\\$ident\\$entry\n";
       }
 }
closedir(DIR);
print "\n\n";



my $usage = <<HELP;

widgetpreinstaller
 
            [-help] Display this message

The script generates an IBY file at /epoc32/rom/include/widgetbackupfiles.iby 
that (when included) will place any widgets that have been backed up in the 
emulator environment at /epoc32/data/WidgetPreInstall in the 
ROM image.

HELP

GetOptions( \my %o, 'help', ) or die $usage; die $usage if $o{help};

my $out = substr( $Bin, 0, 2 ) . "/epoc32/rom/include/widgetbackupfiles.iby";

open my $f, '>', $out or die "Invalid file open $out: $!\n";
print $f "
// GENERATED FILE: EDIT WITH CARE
#ifndef __PREINSTALLEDWIDGETS_IBY__
#define __PREINSTALLEDWIDGETS_IBY__
", get_data("$Widgetdir"),
"
#endif
";

print "Wrote $out\n" and exit;

sub get_data {
    my @files;
    find( sub { -f && push @files, $File::Find::name }, shift );
    return map {
        s/.*data\/(.*)/$1/;
        s#/#\\#g;
        "data=\\epoc32\\data\\$_ data\\$_\n";
    } @files;
}



__END__

:endofperl
