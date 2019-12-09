Imports System
Imports System.IO
Imports System.Security.Cryptography
Imports System.Text

Module Module1

    Sub Main()
        Try
            'Create a UnicodeEncoder to convert between byte array and string.
            Dim ByteConverter As New UnicodeEncoding()

            'Create byte arrays to hold original, encrypted, and decrypted data.
            Dim dataToEncrypt As Byte() = ByteConverter.GetBytes("Data to Encrypt")
            Dim encryptedData() As Byte
            Dim decryptedData() As Byte

            'Create a new instance of RSACryptoServiceProvider to generate
            'public and private key data.
            Dim RSA As New RSACryptoServiceProvider()

            'Pass the data to ENCRYPT, the public key information 
            '(using RSACryptoServiceProvider.ExportParameters(false),
            'and a boolean flag specifying no OAEP padding.
            encryptedData = RSAEncrypt(dataToEncrypt, RSA.ExportParameters(False), False)

            Dim fs As New FileStream("TestCert.xml", FileMode.Create)
            Dim sw As New StreamWriter(fs)

            sw.Write(RSA.ToXmlString(True))

            sw.Flush()
            sw.Close()
            fs.Close()

            'Pass the data to DECRYPT, the private key information 
            '(using RSACryptoServiceProvider.ExportParameters(true),
            'and a boolean flag specifying no OAEP padding.
            decryptedData = RSADecrypt(encryptedData, RSA.ExportParameters(True), False)

            'Display the decrypted plaintext to the console. 
            Console.WriteLine("Decrypted plaintext: {0}", ByteConverter.GetString(decryptedData))
        Catch e As ArgumentNullException
            'Catch this exception in case the encryption did
            'not succeed.
            Console.WriteLine("Encryption failed.")
        End Try
    End Sub


    Public Function RSAEncrypt(ByVal DataToEncrypt() As Byte, ByVal RSAKeyInfo As RSAParameters, ByVal DoOAEPPadding As Boolean) As Byte()
        Try
            'Create a new instance of RSACryptoServiceProvider.
            Dim RSA As New RSACryptoServiceProvider()

            'Import the RSA Key information. This only needs
            'toinclude the public key information.
            RSA.ImportParameters(RSAKeyInfo)

            'Encrypt the passed byte array and specify OAEP padding.  
            'OAEP padding is only available on Microsoft Windows XP or
            'later.  
            Return RSA.Encrypt(DataToEncrypt, DoOAEPPadding)
            'Catch and display a CryptographicException  
            'to the console.
        Catch e As CryptographicException
            Console.WriteLine(e.Message)

            Return Nothing
        End Try
    End Function


    Public Function RSADecrypt(ByVal DataToDecrypt() As Byte, ByVal RSAKeyInfo As RSAParameters, ByVal DoOAEPPadding As Boolean) As Byte()
        Try
            'Create a new instance of RSACryptoServiceProvider.
            Dim RSA As New RSACryptoServiceProvider()

            'Import the RSA Key information. This needs
            'to include the private key information.
            RSA.ImportParameters(RSAKeyInfo)

            'Decrypt the passed byte array and specify OAEP padding.  
            'OAEP padding is only available on Microsoft Windows XP or
            'later.  
            Return RSA.Decrypt(DataToDecrypt, DoOAEPPadding)
            'Catch and display a CryptographicException  
            'to the console.
        Catch e As CryptographicException
            Console.WriteLine(e.ToString())

            Return Nothing
        End Try
    End Function

End Module
