import _mysql
import cgi

body = ''' 
<head>

    <title>MusicBuddy Web Interface</title>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/2.2.2/jquery.min.js"></script>
    <!-- Magic jQuery code for making things happen-->
    <script type="text/javascript">
    $(document).ready(function() {
    var songs = {"Somewhere Over The Rainbow":"C Em Am F E7 G", "Viva La Vida": "D E A F#m C#m", "Riptide": "Am G C F"};
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
            data: "title="+$('#title option:selected').text()+"&action=remove",
        });
    });

    $("#add").click(function(){
        $.ajax({
        url: 'http://iesc-s2.mit.edu/student_code/aladetan/dev1/sb1.py/',
        method:"POST",
        type:"POST",
        data: "title="+$("#new_title").val()+"&chords="+$("#new_chords").val()+"&action=add",
        success: function(data){
            $("#new_title").val("");
            $("#new_chord").val("");
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

class Database():
    #Class used to interact with the database

    def __init__(self):
        #Tries to get connection
        try:
            self.connection = _mysql.connect(host="iesc-s2.mit.edu",user="aladetan_matiash",passwd="TYZfuR5p",db="aladetan_matiash")
            #print(self.connection) #Used to Debug

            #Dictionary that hold all of the song name and their respective chords
            self.songs_to_chords = {}
            
            
        except _mysql.Error as  e:
            print("ERROR! %d: %s" % (e.args[0],e.args[1]))

    def find_songs(self,username):
        '''
        Returns all songs in the songs table that have a specific username
        '''
        query = ("SELECT song_title FROM songs WHERE username='%s'" % username)
        self.connection.query(query)
        result = self.connection.store_result()
        rows = result.fetch_row(maxrows=0,how=0)
        print("These are the songs that we've found for " + username + ":")
        for row in rows:
            print(row)
            
    def add_to_songs(self,ID,username,song_title,chords):
        '''
        Inserts data into the songs table, where data is a list of values specific to that table
        '''
        query = ("INSERT INTO songs (id,username,song_title,chords) VALUES (%d,'%s','%s','%s')" % (ID,username,song_title,chords))
        self.connection.query(query)
        self.connection.commit()
        
    def print_table(self,table):
        '''
        Prints all contents of a specific table
        '''
        query = ("select * from %s" % table)
        self.connection.query(query)
        result = self.connection.store_result()
        rows = result.fetch_row(maxrows=0,how=0)
        for row in rows:
            print(row)

    def reset_table(self,table):
        '''
        Deletes everything from a specific table
        '''
        query = ("DELETE from %s" % table)
        self.connection.query(query)
        self.connection.commit()

    def get_song_chords(self, username):
        '''
        Returns a dictionary of the user's songs mapped to their respective chords
        '''
        query = ("SELECT song_title, chords FROM songs WHERE username = '%s'" % username)
        self.connection.query(query)
        result = self.connection.store_result()
        rows = result.fetch_row(maxrows=0,how=0)
        for row in rows:
            if row[0] not in self.songs_to_chords:
                self.songs_to_chords[row[0].decode("utf-8")]=row[1].decode("utf-8")
        return self.songs_to_chords

exec(open("/var/www/html/student_code/LIBS/s08libs.py").read())
print( "Content-type:text/html\r\n\r\n")
print("<html>")

method_type = get_method_type() #use this for figuring out if it is a GET or POST!
form = cgi.FieldStorage() #get specified parameters!

users = {"sarah":"loves_uke","matias":"mathon"}

if method_type == "GET": #expecting a GET request.
    if 'username' in form.keys() and 'password' in form.keys():
        username = form['username'].value
        password = form['password'].value
        if users[username] != password:
            print('Sorry :( %s. Your password is not correct. Try again!' %(username))
        else:
            database = Database()
            songs_from_db = database.get_song_chords(username)
            database.connection.close() #DON'T TOUCH THIS   
            print(body)
    else:
        print("You need to specify a user name an password as GET parameters")

if method_type == "POST":
    action = form['action'].value
    if action == "add":
        title = form['title'].value
        chords = form['chords'].value
    elif action == "remove":
        title = form['title'].value 



print("</html>")