#!/bin/bash
chessboard=$1"/chess_gopro/chess.txt"
undistortion_path=$2"/undistortion"
mkdir $undistortion_path -p
: > filelist.txt
for image in $2"/*.jpg"
do
    realpath $image > filelist.txt
done
./chessboard_undistortion $chessboard filelist.txt $undistortion_path

