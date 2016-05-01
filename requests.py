import _mysql
import cgi
import sb2

exec(open("/var/www/html/student_code/LIBS/s08libs.py").read())
print( "Content-type:text/html\r\n\r\n")
print("<html>")

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
                    print(song + ", \n")
                print("</h1>")
                print("<h2>")
                for song in songs_from_db.keys():
                    print(songs_from_db[song] + ", \n")
                print("</h2>")
            if action == 'song-tutor':
                latest = database.get_latest_request()
                pattern = latest[0]
                time = latest[1]

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
            if action == 'song-tutor':
                pass

    else:
        print("You need to specify a user name an password as POST parameters")


database.connection.close() #DON'T TOUCH THIS   
print("</html>")
    