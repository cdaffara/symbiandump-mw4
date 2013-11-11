#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Updates version.h information with current date.
#

use strict;
use Cwd;
use POSIX qw(strftime);

my $PLATFORM_LONG = "5_0";
my $PLATFORM_SHORT = "50";

sub update_version_h
{
	my ($version_path, $platform) = @_;
	
	# Check path
	if (! -e $version_path)
	{
		die "version.h can't be found from $version_path";
	}
	
	# Form version strings
	my $version = strftime($platform."_%y%m%d", localtime());
	my $builddate = localtime();
	
	# Backup
	if (-e "$version_path.BACKUP")
	{
		chmod 0666, "$version_path.BACKUP";
		unlink "$version_path.BACKUP";
	}
	chmod 0666, $version_path;
	rename $version_path, "$version_path.BACKUP";
	
	my ($infile, $outfile);
	open $infile, "<$version_path.BACKUP" or die "Can't open $version_path.BACKUP : $!";
	open $outfile, ">$version_path" or die "Can't open $version_path : $!";
	while (<$infile>)
	{
		my $line = $_;
		my $pattern1 = '_LIT\(KVersion.*?;';
		my $replacement1 = "_LIT(KVersion, \"$version\");";
		
		my $pattern2 = '_LIT\(KBuildDate.*?;';
		my $replacement2 = "_LIT(KBuildDate, \"$builddate\");";
		
		if ($line =~ s/$pattern1/$replacement1/g)
		{
			print "Updated KVersion\n";
		}
		elsif ($line =~ s/$pattern2/$replacement2/g)
		{
			print "Updated KBuildDate\n";
		}
		print $outfile $line;
	}
	close $infile;
	close $outfile;
}


my $oldpwd = getcwd();
chdir "..";

### VERSION.H ###########

# version.h
update_version_h(
    'vpnengine/vpncommon/inc/version.h',
    $PLATFORM_LONG);

chdir $oldpwd;
