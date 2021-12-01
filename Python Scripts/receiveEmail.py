import email
import imaplib
import smtplib
from email.mime.text import MIMEText
import time

myEmail = ""
myPassword = ""
emailServer = "imap.gmail.com"
receiverEmail = ""

print("Receiving Emails")
time.sleep(1)
mail = imaplib.IMAP4_SSL(emailServer)
mail.login(myEmail, myPassword)
mail.select('inbox')
status, data = mail.search(None, '(FROM "'+ receiverEmail + '" SUBJECT "BLACK FRIDAY SALE!")')
mail_ids = []

for block in data:
    mail_ids += block.split()

for i in mail_ids:
    status, data = mail.fetch(i, '(RFC822)')

    for response_part in data:
        if isinstance(response_part, tuple):
            message = email.message_from_bytes(response_part[1])
            mail_from = message['from']
            mail_subject = message['subject']

            if message.is_multipart():
                mail_content = ''

                for part in message.get_payload():
                    if part.get_content_type() == 'text/plain':
                        mail_content += part.get_payload()
            else:
                mail_content = message.get_payload()

            print(f'Content: {mail_content}')