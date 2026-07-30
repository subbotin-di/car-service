CREATE MASTER KEY ENCRYPTION BY
PASSWORD= 'your_password'

CREATE CERTIFICATE certificate
WITH SUBJECT = 'Сертификат для шифрования персональных данных';

CREATE SYMMETRIC KEY symmetric_key
WITH ALGORITHM = AES_256
ENCRYPTION BY CERTIFICATE certificate;

OPEN SYMMETRIC KEY symmetric_key
DECRYPTION BY CERTIFICATE certificate;

--CLOSE SYMMETRIC KEY symmetric_key;

--DROP SYMMETRIC KEY symmetric_key 