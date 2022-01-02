<html>

<head>
    <title> Windspeed </title>

</head>

<body>

<?php

    $servername = "localhost";
    $username = "";                  //username database
    $password = "";                  //password database
    $dbname = "anemometro";          //nome database
  
    $conn = new mysqli($servername, $username, $password, $dbname);


    $query = "SELECT * FROM monitoraggio";
    $result = $conn->query($query);



    echo '<table border="8" align="center" cellspacing="2" cellpadding="2"> 
    <tr> 
        <td width="200">  <font face="Arial" align="center" color="red">Timestamp</font> </td> 
        <td width="200">  <font face="Arial" align="center" color="red">Windspeed</font> </td> 
    </tr>';

    if ($result) {
        while ($row = $result->fetch_assoc()) {
            $field1name = $row["timeMYSQL"];
            $field2name = $row["windspeed"];

        echo '<tr> 
                    <td>'.$field1name.'</td> 
                    <td>'.$field2name.'</td> 
                </tr>';
    }
    $result->free();
    } 

?>

</body>

</html>