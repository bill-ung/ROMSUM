<?php
/////////////////////////////////////////////////////////////////////////////
// ROMSUM - Pinball ROM Identifier (web-based tool)
// Written by Bill Ung <bill@ufopinball.com>, Copyright (C) 1996-2018
//
// A web-based tool that allows a user to upload a pinball ROM file that
// they want to verify, or in some cases may not be able to identify.
// Displays size and checksum, plus any matching game information from
// the known database.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////////

// TODO: Update $target_dir to point to your directory
$target_dir = "<http root>/uploads/";
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
$target_name = basename($_FILES["fileToUpload"]["name"]);
$uploadOk = 1;
// Make sure user clicked "submit" button (as opposed to other buttons)
if(isset($_POST["submit"])) {
    $uploadOk = 1;
}
// Check file size
if ($_FILES["fileToUpload"]["size"] > 2097152) {
    echo "Sorry, your file is too large.";
    $uploadOk = 0;
}
// Check for upload errors
if ($uploadOk == 0) {
    echo "Sorry, your file was not uploaded.";
// If everything is ok, try to upload file
} else {
    if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
        // Post results
        include("result.html");
    } else {
        echo "Sorry, there was an error uploading your file.";
        print_r(error_get_last());
    }
}
?>
