<html>

<head>
    <title> Stato real time </title>

</head>

<body>

<?php

    $servername = "localhost";
    $username = "root";
    $password = "pippo";
    $dbname = "testsensore";
  
    $conn = new mysqli($servername, $username, $password, $dbname);


    $query = "SELECT * FROM monitoraggio";
    $result = $conn->query($query);



    echo '<table border="3" align="center" cellspacing="2" cellpadding="2"> 
    <tr> 
        <td> <font face="Arial" align="center" color="red">Timestamp</font> </td> 
        <td> <font face="Arial" align="center" color="red">Stato led</font> </td> 
    </tr>';

    if ($result) {
        while ($row = $result->fetch_assoc()) {
            $field1name = $row["timeNTC"];
            $field2name = $row["stato"];

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
