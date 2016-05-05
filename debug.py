import sb2

print( "Content-type:text/html\r\n\r\n")
print('<html>')

#This a script I can run to debug my POST requests and see what's in the database


database = sb2.Database()
# database.reset_table("songs")
# database.add_to_songs("sarah","The Song","A B C D")
# database.add_to_songs("sarah","Somewhere Over The Rainbow","C Em Am F E7 G")
# database.add_to_songs("sarah","Riptide","Am G C F")
print("Dictionary returned from calling get_song_chords for user 'sarah':")
print(database.get_song_chords("sarah"))
print("<br> <br>")
print("Dictionary returned from calling get_song_chords for user 'matias':")
print(database.get_song_chords("matias"))
print("<br> <br>")
print("Song Database:")
database.print_table("songs")
print("<br> <br>")
# database.send_to_request("0003")
# database.send_to_request("2100")
# database.send_to_request("2000")

print("Latest request in database:")
print(database.get_latest_request())



database.connection.close() #KEEP THIS

print("</html>")