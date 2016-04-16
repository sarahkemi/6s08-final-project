import _mysql
import cgi
import sb2

body = ''' 
<head>

    <title>MusicBuddy Web Interface</title>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/2.2.2/jquery.min.js"></script>
    <!-- Magic jQuery code for making things happen-->
    <script type="text/javascript">
    $(document).ready(function() {
    //var songs = {"Somewhere Over The Rainbow":"C Em Am F E7 G", "Viva La Vida": "D E A F#m C#m", "Riptide": "Am G C F"}; - TEST SONGS
    var songs = %s;
    $.each(songs, function(key, value) {   
     $('#songs')
          .append($('<option>', { value : key })
          .text(key)); 
    });

    //dynamically load song title and chords when song is selected
    $("#songs").change( function(){
        $("#title").text($(this).find(':selected').text());
        $("#chords").text(songs[$(this).find(':selected').text()]);

    });

    $("#remove").click(function(){
        $.ajax({
            url: 'http://iesc-s2.mit.edu/student_code/aladetan/dev1/sb1.py/',
            method:"POST",
            type:"POST",
            data: "title="+$('#songs option:selected').text()+"&action=remove&username=%s",
            success: function(){
            setTimeout(function(){ location.reload(); }, 200);
            },
        });
    });

    $("#add").click(function(){
        $.ajax({
        url: 'http://iesc-s2.mit.edu/student_code/aladetan/dev1/sb1.py/',
        method:"POST",
        type:"POST",
        data: "title="+$("#new_title").val()+"&chords="+$("#new_chords").val()+"&action=add&username=%s",
        success: function(){
            $("#new_title").val("");
            $("#new_chords").val("");
            setTimeout(function(){ location.reload(); }, 200);
        },
        });
    });

    });
    </script>
    <style type="text/css">
    *{
    list-style:none;
    padding:0;
    }
    body{
    background-color:white;
    font-family:arial;
    font-size:10pt;
    color:#999999;
    }
    h2{
    font-family:arial;
    font-size:14pt;
    color:#3399cc;
    }
    h3{
    font-family:arial;
    font-size:12pt;
    color:#3399cc;  
    }
    .wrapper{
    width:600px;
    margin: 0 auto;
    }
    #left{
    float:left;
    width:290px;
    padding-right:10px;
    }
    #right{
    float:right;
    width:300px;
    }
    .clear{
        clear:both;
    }
    select{
        width:250px;
    }
    </style>
</head>
<body>
    <div class="wrapper">
    <h1>MusicBuddy</h1>
    <div id="left">
        <h2>Songs:</h2>
        <select size=10 id="songs">
        </select>
    </div>
    <div id="right">
        <h3>Title: </h3> <p id="title">[Select a Song]</p>
        <h3>Chords:</h3> <p id="chords">[Select a Song]</p>
        <br>
        <button type="button" id="remove">Remove Song</button>
    </div>
    <div class="clear"></div>
    </div>

    <div class="wrapper">
    <h2>Add New Song:</h2>
    Title: <input type="text" id="new_title"></input>
    Chords: <input type="text" id="new_chords"></input>
    <button type="button" id="add"> Add Song</button>
    </div>

</body>
'''


exec(open("/var/www/html/student_code/LIBS/s08libs.py").read())
print( "Content-type:text/html\r\n\r\n")
print("<html>")

method_type = get_method_type() #use this for figuring out if it is a GET or POST!
form = cgi.FieldStorage() #get specified parameters!

users = {"sarah":"loves_uke","matias":"mathon"}

database = sb2.Database()

if method_type == "GET": #expecting a GET request.
    if 'username' in form.keys() and 'password' in form.keys():
        username = form['username'].value
        password = form['password'].value
        if users[username] != password:
            print('Sorry :( %s. Your password is not correct. Try again!' %(username))
        else:
            songs_from_db = database.get_song_chords(username)
            print(body %(str(songs_from_db),username,username)) #duplicate usernames are for add and remove jquery, to insert the username into the post request
    else:
        print("You need to specify a user name an password as GET parameters")

if method_type == "POST":
    action = form['action'].value
    username = form['username'].value
    if action == "add":
        title = form['title'].value
        chords = form['chords'].value

        database.add_to_songs(username,title,chords)


    elif action == "remove":
        title = form['title'].value

        database.remove_song(username,title)

database.connection.close() #DON'T TOUCH THIS   

print("</html>")