import sb2

print( "Content-type:text/html\r\n\r\n")
print('<html>')

#This a script I can run to debug my POST requests and see what's in the database


database = sb2.Database()
# database.reset_table("songs")
# database.add_to_songs("sarah","The Song","A B C D")
# database.add_to_songs("sarah","Somewhere Over The Rainbow","C Em Am F E7 G")
# database.add_to_songs("sarah","Riptide","Am G C F")
print("Dictionary returned from calling get_song_chords:")
print(database.get_song_chords("sarah"))
print("<br> <br>")
print("Song Database:")
database.print_table("songs")



database.connection.close() #KEEP THIS

print("</html>")