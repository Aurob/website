import re, json


txt = open("bartletts.txt", "r").read()

# return each author found by regex search
def getAuthors():
    pattern = '(([A-Z]{3,})+(.*?)[0-9]+-[0-9]+\.)'
    quotes = re.findall(pattern, txt)

    authors = list(map(lambda x: x[0].strip(), quotes))

    author_obj = []
    for unparsed_author in authors:
        parsed_author = parseAuthor(unparsed_author)
        author_obj.append(parsed_author)
        
    return author_obj

# return formatted name and year
def parseAuthor(author):
    data = author.split(".")        
    name = '.'.join(data[:-2])
    first_name = name.split(" ")[0]
    last_name = name.split(" ")[len(name.split(" "))-1]
    
    year = data[len(data)-2].strip().split("-")
    year_start = year[0]
    year_end = year[1]
    
    return {
        "author_first":first_name,
        "author_last":last_name,
        "year_start":year_start,
        "year_end":year_end
        }

def getQuotes():
    return ""

def getFootnotes():
    return ""

'''
author_full_quotes = []
for a in range(len(authors)-1):
    author = authors[a]

    start = txt.find(author)
    end = txt.find(authors[a+1])
    if end == -1:
        end = len(author)
    full_text = txt[start+len(author):end]
    note_list = {}
    fndex = full_text.find("FOOTNOTES:")
    if fndex > -1:
        quotes = full_text[:fndex]
        
        footnotes = full_text.split("FOOTNOTES:")[1]
        foot_pattern = '\[[0-9]{1,3}-[0-9]{1,3}\]'
        notes = re.findall(foot_pattern, footnotes)

        for n in range(len(notes)-1):
            start = footnotes.index(notes[n])
            end = footnotes.index(notes[n+1])
            note = footnotes[start+len(notes[n]):end]
            note_list[notes[n]] = note.strip()
    
    else:
        quotes = full_text
    
    input(note_list)
    
    each_quote = re.findall("([\(]?_(.*?)\._[\)]?)", quotes)
    quote_list = []
    for q in range(len(each_quote)-1):
        source = each_quote[q][0]
        start = quotes.find(source)
        end = quotes.find(each_quote[q+1][0])
        quote = quotes[start+len(source):end]
        quote_list.append(quote.strip())

        
    author_full_quotes.append({
        "author":author,
        "quotes":quote_list,
        "footnotes": note_list
    })

open("bartlett_quotes.json", "w").write(json.dumps(author_full_quotes))
'''
