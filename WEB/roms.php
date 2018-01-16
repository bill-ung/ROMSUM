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

// Hide database credentials
include 'credentials.php';

// Get the HTTP method, path and body of the request
$method = $_SERVER['REQUEST_METHOD'];
$request = explode('/', trim($_SERVER['PATH_INFO'],'/'));
 
// Connect to the MYSQL database
$link = mysqli_connect($sql_server, $sql_username, $sql_password, $sql_database);
mysqli_set_charset($link,'utf8');
 
// Retrieve parameters from the path and sanitize input
$size = preg_replace('/[^0-9]/','',$request[0]);
$csum = preg_replace('/[^A-Za-z0-9]/','',$request[1]);
$size = (strlen($size) > 7) ? substr($size,0,7) : $size;
$csum = (strlen($csum) > 4) ? substr($csum,0,4) : $csum;

// Execute SQL statement
$sql = "select * from roms WHERE Size=$size AND Checksum='$csum'";
$result = mysqli_query($link,$sql);
 
// Check if SQL statement failed
if (!$result) {
  // Return error
  http_response_code(404);
  die(mysqli_error());
} else {
  // Output database results in JSON
  for ($i=0;$i<mysqli_num_rows($result);$i++) {
    echo ($i>0?',':'').json_encode(mysqli_fetch_object($result));
  }
}
 
// Close MYSQL connection
mysqli_close($link);

?>
