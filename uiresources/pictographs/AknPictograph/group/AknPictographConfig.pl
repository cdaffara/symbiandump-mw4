#
# Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
#    Configures the pictograph building environment during the build process.
#
use strict;
use File::Path;
my ($myname, $mydir);
#
#  Determine where this program is
#
BEGIN {
  ($myname = $0) =~ s@.*(\\|/)@@;
  ($mydir = $0)  =~ s@(\\|/)[^(\\|/)]*$@@;
  $mydir =~ s@\\@/@g; #replace \ with /
  $mydir = "." if ($mydir eq $myname); 
}         

#constants for directory paths
use constant CONFIG_ROOT_DIR					 			=> "$mydir/../config";
use constant CONFIG_FILE_NAME					 			=> "config.ini";
use constant PICTOGRAPH_BITMAPS_ROOT	 			=> "$mydir/../../aknpictographbitmaps";
use constant PICTOGRAPH_BITMAPS_DATA	 			=> PICTOGRAPH_BITMAPS_ROOT."/data";
use constant GENERATED_FILES_DIR     	 			=> "$mydir/../generated";

# supported keys in config.ini
use constant PICT_HEIGHTS  						 			=> "PICTOGRAPH_HEIGHTS";
use constant PICTOGRAPH_BITMAPS_SOURCE_DIR  => "PICTOGRAPH_BITMAPS_SOURCE_DIR";
use constant PICT_BITMAP_DISPLAY_MODE  			=> "PICTOGRAPH_BITMAP_DISPLAY_MODE";
use constant PICT_BITMAP_MASK_DISPLAY_MODE  => "PICTOGRAPH_BITMAP_MASK_DISPLAY_MODE";

# supported display modes for pictograph bitmaps/masks
use constant ECOLOR256  										=> "EColor256";
use constant ECOLOR64K  										=> "EColor64K";
use constant EGRAY2        									=> "EGray2";
use constant EGRAY256      									=> "EGray256";

#heade file template related
use constant COLORMODE_BITMAP_TAG_VALUE   	=> "COLORMODE_BITMAP_TAG_VALUE";
use constant COLORMODE_MASK_TAG_VALUE     	=> "COLORMODE_MASK_TAG_VALUE";
use constant DISPLAY_MODE_BITMAP_VALUE   	  => "DISPLAY_MODE_BITMAP_VALUE";
use constant DISPLAY_MODE_MASK_VALUE      	=> "DISPLAY_MODE_MASK_VALUE";
use constant CONF_HEADER_FILE_NAME					=> "aknpictographconfig.h";
use constant CONF_HEADER_FILE_NAME_DEF			=> "AKNPICTOGRAPHCONFIG_H";
use constant CONF_HEADER_FILE_TEMPLATE			=>
				"// This file is generated by AknPictographConfig.pl. Do not edit!!\n\n\n\n\n".
				"#ifndef @{[CONF_HEADER_FILE_NAME_DEF]}\n".
				"#define @{[CONF_HEADER_FILE_NAME_DEF]}\n\n".
				"#include <gdi.h>\n\n".
				"// Color mode tags for BMCONV tool\n".
				"_LIT8( KBitmapTag, @{[COLORMODE_BITMAP_TAG_VALUE]} );\n".
				"_LIT8( KMaskTag, @{[COLORMODE_MASK_TAG_VALUE]} );\n\n".
				"// Display modes for pictograph bitmap/mask creation\n".
				"const TDisplayMode KBitmapDisplayMode = @{[DISPLAY_MODE_BITMAP_VALUE]};\n".
				"const TDisplayMode KMaskDisplayMode   = @{[DISPLAY_MODE_MASK_VALUE]};\n\n".
				"#endif // @{[CONF_HEADER_FILE_NAME_DEF]}\n\n".
				"// End of file\n";

# hash for supported display modes for pictograph bitmaps
my %supported_bitmap_display_modes = 
						(
						uc (ECOLOR256) => +ECOLOR256,
						uc (ECOLOR64K) => +ECOLOR64K
						);			
# hash for supported display modes for pictograph masks
my %supported_bitmap_mask_display_modes = 
						(
						uc (EGRAY2)   => +EGRAY2,
						uc (EGRAY256) => +EGRAY256
						);																
						
# hash for supported bitmap/mask color mode tags for BMCONV tool
my %supported_color_mode_tags = 
						(
						@{[ECOLOR256]}   => qq("/c8"),
						@{[ECOLOR64K]}   => qq("/c16"),
						@{[EGRAY2]}      => qq("/1"),
						@{[EGRAY256]}    => qq("/8")						
						);																

my $usage = "Usage:\n".
				    "   perl AknPictographConfig.pl <config_dir_name>\n\n".
				    "   <config_dir_name> = directory below directory '<location of AknPictographConfig.pl>/../config'\n\n";
  				    
###############################################################################
# main code
###############################################################################				    
my $config_dir_name = shift || die $usage;
$config_dir_name = lc $config_dir_name;

my $values_ref = ParseConfigFile($config_dir_name);
PrintConfigInfo($values_ref,$config_dir_name);
GenerateConfigHeader($values_ref);
PrepareBuildEnvironment($values_ref,$config_dir_name);

###############################################################################
# subroutines
###############################################################################				    

#
# Subroutine for parsing pictograph configuration file 
#
# parameters 
# $_[0] - name of the configuration directory under CONFIG_ROOT_DIR
#
# return: reference to a hash of key value pairs 
#
sub ParseConfigFile{
	my $config_dir_name = shift || die "ParseConfigFile: argument error!";
	my $filename = CONFIG_ROOT_DIR."/$config_dir_name/".CONFIG_FILE_NAME;
	my $content;
	my %values;
	open(FILE, $filename) || die "Unable to open file $filename: $!";
	{                                                         
	  local $/ ;                                            
	  undef $/ ;  # reads the whole file on the next line   
	  $content = <FILE> ;                                         
	}             
	close FILE;
	
	#remove comments from $content
	#Note: comments start with // and ends with line end
	$content =~ s@//.*?$@@mg;
			
	# Regular expression for matching a token after '=' in $1.
	# The token is trimmed from left and right
	my $match_token = '[^\S\n]*=[^\S\n]*(\w.*?)[^\S\n]*$';
	
	#match PICT_HEIGHTS key value pair
	#value is list of heights
	my $tmp = PICT_HEIGHTS;
	$content =~ /$tmp$match_token/mi || die "$tmp or value is not specified in $filename !\n";
	$_=$1;
	@{$values{$tmp}} = split;
	die "At least a single pictograph height must be specified for $tmp in $filename\n" if @{$values{$tmp}}==0;
	
	#match PICTOGRAPH_BITMAPS_SOURCE_DIR key value pair
	$tmp = PICTOGRAPH_BITMAPS_SOURCE_DIR;
	$values{$tmp} = $config_dir_name; #default value
	$values{$tmp} = $1 if ($content =~ /$tmp$match_token/mi);
	
	#match PICT_BITMAP_DISPLAY_MODE key value pair
	$tmp = PICT_BITMAP_DISPLAY_MODE;
	$values{$tmp} = ECOLOR256; #default value
	if ($content =~ /$tmp$match_token/mi){
		$_ = $supported_bitmap_display_modes{uc $1} || die "Value \'$1\' is not supported for $tmp!\n";
		$values{$tmp} = $_;
	}
	
	#match PICT_BITMAP_MASK_DISPLAY_MODE key value pair
	$tmp = PICT_BITMAP_MASK_DISPLAY_MODE;
	$values{$tmp} = EGRAY2; #default value
	if ($content =~ /$tmp$match_token/mi){
		$_ = $supported_bitmap_mask_display_modes{uc $1} || die "Value \'$1\' is not supported for $tmp!\n";
		$values{$tmp} = $_;
	}
	return \%values;
}

#
# Subroutine for preparing build environment for building pictographs
# bitmaps.
#
# parameters 
# $_[0] - reference to a hash of key value pairs
# $_[1] - name of the configuration directory under CONFIG_ROOT_DIR
#				    
sub PrepareBuildEnvironment{
	die "PrepareBuildEnvironment: argument error!" if @_!=2;
	my ($values_ref,$config_dir_name) = @_;		
	my $bitmap_target = PICTOGRAPH_BITMAPS_DATA;
	$bitmap_target =~ s@/@\\@g; #replace '/' with '\\' for Xcopy, del
	my $config_source = CONFIG_ROOT_DIR;
	$config_source =~ s@/@\\@g; #replace '/' with '\\' for Xcopy, del
	
	print "Cleanup...\n";
	
	system("del /q /f $bitmap_target\\*.* > nul 2>&1");
	system("del /q /f $config_source\\*.* > nul 2>&1");
	
	my @pict_height_list = @{$values_ref->{+PICT_HEIGHTS}};
	print "Extracting pictographs for $config_dir_name with heights: @pict_height_list in progress...\n";
	
	my $command = "unzip -o -qq @{[PICTOGRAPH_BITMAPS_ROOT]}/$values_ref->{+PICTOGRAPH_BITMAPS_SOURCE_DIR}/*.zip -d @{[PICTOGRAPH_BITMAPS_DATA]}";
	system("$command > nul 2>&1") == 0 or	die qq(Command "$command" has failed: $?\n);
			
	foreach (@pict_height_list){
		my $command = "xcopy $config_source\\$config_dir_name\\picto"."$_".".ini $config_source\\";
		system("$command > nul 2>&1") == 0 or	die qq(Command "$command" has failed: $?\n);	
	}	
	print "Configuring build environment is successful.\n";
}

#
# Subroutine for generating configuration header file
#
# parameters 
# $_[0] - reference to a hash of key value pairs
#				    
sub GenerateConfigHeader{
	my $values_ref = shift || die "GenerateConfigHeader: argument error!";
	my $conf_header_content = CONF_HEADER_FILE_TEMPLATE;
	
	my $tmp = $values_ref->{+PICT_BITMAP_DISPLAY_MODE};
	$conf_header_content =~ s/@{[DISPLAY_MODE_BITMAP_VALUE]}/$tmp/si; #insert bitmap display mode
  $conf_header_content =~ s/@{[COLORMODE_BITMAP_TAG_VALUE]}/$supported_color_mode_tags{$tmp}/si; #insert bitmap color mode tag
	$tmp = $values_ref->{+PICT_BITMAP_MASK_DISPLAY_MODE};
	$conf_header_content =~ s/@{[DISPLAY_MODE_MASK_VALUE]}/$tmp/si; #insert mask display mode
  $conf_header_content =~ s/@{[COLORMODE_MASK_TAG_VALUE]}/$supported_color_mode_tags{$tmp}/si; #insert mask color mode tag
  
  mkpath GENERATED_FILES_DIR;
  $tmp = GENERATED_FILES_DIR."/".CONF_HEADER_FILE_NAME;
  open(OUTFILE, ">$tmp") || die "Unable to create file $tmp: $!";
	print OUTFILE $conf_header_content;
	close OUTFILE;                                                    	
}

# Subroutine for printing info to stdout about the current 
# pictograph build configuration
#
# parameters 
# $_[0] - reference to a hash of key value pairs
# $_[1] - name of the configuration directory under CONFIG_ROOT_DIR
#				    
sub PrintConfigInfo{
	die "PrintConfigInfo: argument error!" if @_!=2;
	my ($values_ref,$config_dir_name) = @_;		
	print "Pictograph configuration dir:       $config_dir_name\n";
	print "Pictographs heights:                @{$values_ref->{+PICT_HEIGHTS}}\n";
	print "Pictograph bitmaps color mode:      $values_ref->{+PICT_BITMAP_DISPLAY_MODE}\n";
	print "Pictograph bitmap masks color mode: $values_ref->{+PICT_BITMAP_MASK_DISPLAY_MODE}\n";	
}