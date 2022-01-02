<?php

if(isset($_GET["windspeed"])) {
   $stato = $_GET["windspeed"]; 

   $servername = "localhost";
   $username = "";                  //username database
   $password = "";                  //password database
   $dbname = "anemometro";          //nome database

   // Create connection
   $conn = new mysqli($servername, $username, $password, $dbname);
   // Check connection
   if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
   }

   //creare una tabella "monitoraggio" con due colonne "timeMYSQL" e "windspeed" o modificare i campi
   //della query sottostante
   $sql = "INSERT INTO monitoraggio (windspeed) VALUES ($stato)";


   if ($conn->query($sql) == TRUE){
      
      echo "New records created successfully \n";
    
   }
      
   else{
      echo "Error: " . $sql . " => " . $conn->error . "\n";
   }

   $conn->close();
} 
else {
   echo "Stato is not set \n";
}
?>
