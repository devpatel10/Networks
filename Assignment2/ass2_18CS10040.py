#########################################################################
#               Name : Patel Devarshi Chandrakant                       #
#               Roll No: 18CS10040                                      #
#               Computer Networks Lab Assignment 2                      #
#########################################################################

#   LOGIC:
#   Sample XML code 
#   <pdml>
#   <packet>
#   <proto name="http">
#   <field name="http.x_forwarded_for" showname="X-Forwarded-For: 123.108.246.22\r\n" size="33" pos="435" show="123.108.246.22" value="582d466f727761726465642d466f723a203132332e3130382e3234362e32320d0a"/>
#   <field name="http.request.line" showname="Via: Internet.org\r\n" size="19" pos="518" show="Via: Internet.org" value="5669613a20496e7465726e65742e6f72670d0a"/>
#   </proto>
#   </packet>
#   </pdml>
#
#   We are particularly interested in the above mentioned two field tags
#   Field tag from line no 12 will give us the ip address of the original source
#   Field tag from line 13 will make sure the request is via Internet.org

from geolite2 import geolite2
import operator
from lxml import etree
import csv
import xml.dom.minidom 
import sys



#taking input file address from command argument
inputfile = sys.argv[1]

#generating parser of the xml file
parser=etree.parse(inputfile)

#root node initialization
root=parser.getroot()

#stores the entire list of source ips requesting via internet.org proxy
list_of_ips=[]

#iterating through the childern and nested children for type proto="http"
for i in root.iter("proto"):
    if i.get('name')=="http":
        ip=""   #to store source ip
        flag=0  #to indicate proxy is via Internet.org
        #will iterate through the field tags
        for j in i.iter("field"):
            if j.get('name')=="http.x_forwarded_for":
                #will save the ip to a variable 
                ip+= str(j.get('show'))
            #conditions to check Internet.org proxy
            if j.get('name')=="http.request.line":
                if j.get('showname')=="Via: Internet.org\\r\\n":
                    flag=1
        #Addying source IP to the list only if the request is via Internet.org
        if flag==1 :
            list_of_ips.append(str(ip))

#removing repetitive ips for distinct ips
unique_ips=set(list_of_ips)

#saves the Country : Frequency in form of dictionary
freq_loc={}

reader=geolite2.reader()
for ips in unique_ips:
    #locations contains entire info which can be deduced from IP and geolite2 library
    location=reader.get(ips)
    #fetching the country from whole set of details 
    country=location.get('country').get('names').get('en')
    #incrementing the frequency of each country
    if (country in freq_loc):
        freq_loc[country]+=1
    else:
        freq_loc[country]=1


#sorting in decreasing order
freq_loc=dict( sorted(freq_loc.items(), key=operator.itemgetter(1),reverse=True))


#To give heading to the output file
field_names=['Country Name','Frequency of requests']

#modifying the freq dict to make CSV file
final_dict = [(word.upper(),freq_loc[word]) for word in freq_loc]

#writing in output csv file
with open('Frequency_Output_18CS10040.csv', 'w') as csvfile: 
    writer = csv.writer(csvfile) 
    writer.writerow(field_names) 
    writer.writerows(final_dict) 
