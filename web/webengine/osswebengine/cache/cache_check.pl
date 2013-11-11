#!/usr/bin/perl

#use Encode;

# open index file
open INH,'<','\epoc32\winscw\c\system\cache\index.dat' or die "couldn't open index.dat";
binmode INH;

my @missing_files;
my @missing_urls;

my @cache_uris;
my @cache_files;

my @dir_items = GetDirContents();
my @cache_content = ParseCacheIndex();
my @extra_files;
GetExtraFiles();

$count = 0;
print "\n\nMissing Items:\n";
foreach (@missing_files) {
  print "$_ - $missing_urls[$count++]\n";
}

print "\n\nExtra Files:\n";
foreach (@extra_files) {
  print "$_\n";
}




sub GetDirContents
{
  my @subdirectories = ( "0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f" );
  my @diritems = ();
  $index=0;
  foreach (@subdirectories) 
  {
    opendir DIRHANDLE,"\\epoc32\\winscw\\c\\system\\cache\\@subdirectories[$index++]" or die "couldn't open cache directory";
    my @tmp = readdir DIRHANDLE;
    push @diritems, @tmp;
  }

  return @diritems;
}

sub ParseCacheIndex
{
  $filenames;
  $numEntries = GetNumEntries();

  print "Index contains $numEntries items\n";

  while($numEntries)
  {
    @urifilenamearray = GetNextFilename();
    print "@urifilenamearray[1] - @urifilenamearray[0]\n";
    push @filenames, @urifilenamearray[0];
    push @filenames, @urifilenamearray[1];
    $numEntries--;
    CheckFilePresent(@urifilenamearray[1], @urifilenamearray[0]);
    push @cache_files, @urifilenamearray[1];
    push @cache_urls, @urifilenamearray[0];
  }  

  return @filenames;
}

sub GetNumEntries
{
  read (INH, $buffer, 4);  #throw away version number
  read (INH, $buffer, 4);  # read num of 16-bit chars in dir stub
  $charcount = unpack("l",$buffer);
  read (INH, $buffer, $charcount*2); #throw away dir stub
  read (INH, $buffer, 4); # read entry count
  $value = unpack("l",$buffer);
  return $value;
}

sub GetNextFilename
{
  my @return = ();

  read (INH, $buffer, 4);
  # get URI
  $stringlen = unpack("l",$buffer);
  read (INH, $buffer, $stringlen);

  @return[0] = $buffer;
#  print "$stringlen chars in URI:\n$buffer\n";

  # get unicode filename
#  read (INH, $buffer, 4);
#  $stringlen = unpack("l", $buffer);

  $stringlen = 8;
  my @filename_array = ();
  if($stringlen)
  {
    # convert $buffer from unicode to ascii
    while($stringlen) {
      read (INH, $buffer, 2);
      $string = unpack("a",$buffer);
      #print "$string";
      push @filename_array, $string;
      $stringlen--;
    }
  }
  @return[1] = join '',@filename_array;
  #print "\n\n";
  # gobble rest of item up to header data
  read (INH, $buffer, 24);

  read (INH, $buffer, 4);
  $stringlen = unpack("l",$buffer);
  read (INH, $buffer, $stringlen);	# throw away header data.

  return @return;
}

sub CheckFilePresent()
{
  my $filename = $_[0];
  my $url = $_[1];
  
  my $fqname = "\\epoc32\\winscw\\c\\system\\cache\\". (substr $filename, -1, 1) . "\\" . $filename;
  
  my $FILE;
  open FILE, $fqname or MissingItem($filename, $url);
}

sub MissingItem()
{
  push @missing_files, $_[0];
  push @missing_urls,  $_[1];
}

sub GetExtraFiles()
{
  # dir_items contains list of all filenames in all directories.
  my $dirnum = -1;
  my @dirnames = ( "0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f" );
  
  foreach $item (@dir_items)
  {
    my $match = 0;
    if($item eq '.')
    {
      $dirnum++;
    }
    elsif($item ne '..') # filter out directories
    {
      foreach (@cache_files)
      {
        $match = 1 if($_ eq $item);
      }
      if($match == 0)
      {
        my $fqname = "\\epoc32\\winscw\\c\\system\\cache\\".@dirnames[$dirnum]."\\".$item;
        push @extra_files, $fqname;
      }
    }
  }
}