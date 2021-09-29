<?php

if(isset($_GET["statoLed"])) {
   $stato = $_GET["statoLed"]; 
   $times = $_GET["timestamp"];

   $servername = "localhost";
   $username = "root";
   $password = "pippo";
   $dbname = "testsensore";

   // Create connection
   $conn = new mysqli($servername, $username, $password, $dbname);
   // Check connection
   if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
   }

   //$sqlDEL = "DELETE FROM monitoraggio";
   $sql = "INSERT INTO monitoraggio (timeNTC,stato) VALUES ($times,$stato)";
   //$sql2 = "INSERT INTO monitoraggio (time) VALUES ($time)";

   if ($conn->query($sql) == TRUE){
      
      echo "New records created successfully \n";
    
   }
      
   else{
      echo "Errore";
      echo "Error: " . $sql . " => " . $conn->error;
   }

   $conn->close();
} else {
   echo "Stato is not set";
}
?>
