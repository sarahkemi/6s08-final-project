import _mysql
import cgi
import sb2
import datetime

print( "Content-type:text/html\r\n\r\n")
print("<html>")

exec(open("/var/www/html/student_code/LIBS/s08libs.py").read())


method_type = get_method_type() #use this for figuring out if it is a GET or POST!
form = cgi.FieldStorage() #get specified parameters!

database = sb2.Database()

users = {"sarah":"loves_uke","matias":"mathon"}


if method_type == "GET":
    if 'username' in form.keys() and 'password' in form.keys():
        username = form['username'].value
        password = form['password'].value
        if users[username] != password:
            print('Sorry :( %s. Your password is not correct. Try again!' %(username))
        else:
            action = form['action'].value
            if action == 'song-finder':
                songs_from_db = database.get_song_chords(username)
                print("<h1>")
                for song in songs_from_db.keys():
                    print(song)
                print("</h1>")
                print("<h2>")
                for song in songs_from_db.keys():
                    print(songs_from_db[song])
                print("</h2>")
            if action == 'song-tutor':
                latest = database.get_latest_request()
                pattern = latest[0]
                #check how long it's been since the last request was made
                now = datetime.datetime.now()
                time_format = '%Y-%m-%d %H:%M:%S'
                time = datetime.datetime.strptime(latest[1], time_format)
                elapsed = now - time
                if elapsed <= datetime.timedelta(minutes=5):
                    print("<p>"+pattern+"</p>")
                else:
                    print("<p>5555</p>")


    else:
        print("You need to specify a user name an password as GET parameters")

if method_type == "POST":
    if 'username' in form.keys() and 'password' in form.keys():
        username = form['username'].value
        password = form['password'].value
        if users[username] != password:
            print('Sorry :( %s. Your password is not correct. Try again!' %(username))
        else:
            action = form['action'].value
            if action == 'send-pattern':
                song = form['song-title'].value
                chords = database.get_song_chords(username)[song] #Sort of inefficient because get_song_chords recreates a dictionary of song:chords mappings, and then we select the same song from that dictionary
                chords_list = chords.split()
                index = form['index'].value
                pattern = database.get_chord_pattern(chords_list[index])
                database.send_to_request(pattern)
                
                
    else:
        print("You need to specify a user name an password as POST parameters")


database.connection.close() #DON'T TOUCH THIS   
print("</html>")
    
