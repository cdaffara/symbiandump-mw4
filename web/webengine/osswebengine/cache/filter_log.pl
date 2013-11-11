#!/usr/bin/perl

open INH,"<",$ARGV[0];
open OUTH,">",$ARGV[1];

foreach $line (<INH>){
  if($line =~ m/CACHEPOSTPONE:/)
  {
    print OUTH $line;
  }
}

