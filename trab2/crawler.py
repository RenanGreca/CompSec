import urllib2
from bs4 import BeautifulSoup

website = 'https://www.letras.mus.br'

response = urllib2.urlopen(website + '/legiao-urbana/')
content = response.read()

encoding = response.headers['content-type'].split('charset=')[-1]
html = unicode(content, encoding)

html_soup = BeautifulSoup(html, 'html.parser')

ul = html_soup.find('ul', class_="cnt-list")

# Gets all items in list of songs
for li in ul.find_all('li'):
    
    # removes illegal character / from filename
    title = li.string.encode('utf-8').replace('/ ', '_')
    print title

    # gets the HTML for the specific page
    res = urllib2.urlopen(website + li.a['href'])

    con = res.read()

    enc = response.headers['content-type'].split('charset=')[-1]
    song_html = unicode(con, enc)

    song_soup = BeautifulSoup(song_html, 'html.parser')

    # strips HTML tags from the lyrics
    lyrics = str(song_soup.article).replace('<article> ', '').replace('</article>', '').replace('<p>', '\n').replace('</p>', '\n').replace('<br/>', '\n')

    # write lyrics to file
    with open("legiaourbana/"+title+".txt", "w") as lyrics_file:
        lyrics_file.write(lyrics)