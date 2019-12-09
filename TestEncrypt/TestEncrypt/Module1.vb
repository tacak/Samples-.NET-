Imports System.Security.Cryptography
Imports System.Text
Imports System.IO

Module Module1

    Sub Main()
        Try
            Dim publicKey As String
            Dim privateKey As String

            Dim CSPParam As New System.Security.Cryptography.CspParameters
            Dim rsa As New _
                System.Security.Cryptography.RSACryptoServiceProvider(CSPParam)

            '公開鍵をXML形式で取得
            publicKey = rsa.ToXmlString(False)
            '秘密鍵をXML形式で取得
            privateKey = rsa.ToXmlString(True)

            Dim fs As New FileStream("pKey.xml", FileMode.Create)
            Dim sw As New StreamWriter(fs)

            sw.Write(privateKey)

            sw.Flush()
            sw.Close()
            fs.Close()

            ' Create a new Rijndael object to generate a key
            ' and initialization vector (IV).
            Dim RijndaelAlg As Rijndael = Rijndael.Create
            RijndaelAlg.KeySize = 128

            ' Create a string to encrypt.
            Dim sData As String = "Here is some data to encrypt."
            Dim FileName As String = "CText.dat"

            fs = New FileStream(FileName, FileMode.Create)
            Dim bw As New BinaryWriter(fs)

            Dim Key(256) As Byte
            Dim IV(256) As Byte

            Key = Encrypt(RijndaelAlg.Key, publicKey)
            IV = Encrypt(RijndaelAlg.IV, publicKey)

            bw.Write(CType(Key.Length, Byte))
            bw.Write(CType(IV.Length, Byte))
            bw.Write(Key)
            bw.Write(IV)

            bw.Flush()
            bw.Close()
            fs.Close()

            ' Encrypt text to a file using the file name, key, and IV.
            EncryptTextToFile(sData, FileName, RijndaelAlg.Key, RijndaelAlg.IV)

            fs = New FileStream(FileName, FileMode.Open)
            Dim br As New BinaryReader(fs)

            fs.Seek(2, SeekOrigin.Begin)
            br.Read(Key, 0, 128)
            br.Read(IV, 0, 128)

            fs.Close()

            fs = New FileStream("pKey.xml", FileMode.Open)
            Dim sr As New StreamReader(fs)

            privateKey = sr.ReadLine()
            sr.Close()
            sr.Close()

            Key = Decrypt(Key, privateKey)
            IV = Decrypt(IV, privateKey)

            ' Decrypt the text from a file using the file name, key, and IV.
            Dim Final As String = DecryptTextFromFile(FileName, Key, IV)

            ' Display the decrypted string to the console.
            Console.WriteLine(Final)
        Catch e As Exception
            Console.WriteLine(e.Message)
        End Try

        Console.ReadLine()

    End Sub

    Public Function Encrypt(ByVal data() As Byte, _
                            ByVal publicKey As String) As Byte()
        'RSACryptoServiceProviderオブジェクトの作成
        Dim CSPParam As New System.Security.Cryptography.CspParameters
        CSPParam.Flags = _
            System.Security.Cryptography.CspProviderFlags.UseMachineKeyStore
        Dim rsa As New _
            System.Security.Cryptography.RSACryptoServiceProvider(CSPParam)

        '公開鍵を指定
        rsa.FromXmlString(publicKey)

        '暗号化する
        '（XP以降の場合のみ2項目にTrueを指定し、OAEPパディングを使用できる）
        Dim encryptedData As Byte() = rsa.Encrypt(data, False)

        'Base64で結果を文字列に変換
        Return encryptedData
    End Function

    Public Function Decrypt(ByVal data() As Byte, _
                            ByVal privateKey As String) As Byte()
        'RSACryptoServiceProviderオブジェクトの作成
        Dim CSPParam As New System.Security.Cryptography.CspParameters
        CSPParam.Flags = _
            System.Security.Cryptography.CspProviderFlags.UseMachineKeyStore
        Dim rsa As New _
            System.Security.Cryptography.RSACryptoServiceProvider(CSPParam)

        '秘密鍵を指定
        rsa.FromXmlString(privateKey)

        '復号化する
        Dim decryptedData As Byte() = rsa.Decrypt(data, False)

        '結果を文字列に変換
        Return decryptedData
    End Function


    Sub EncryptTextToFile(ByVal Data As String, ByVal FileName As String, ByVal Key() As Byte, ByVal IV() As Byte)
        Try
            ' Create or open the specified file.
            Dim fStream As FileStream = File.Open(FileName, FileMode.Append)

            ' Create a new Rijndael object.
            Dim RijndaelAlg As Rijndael = Rijndael.Create

            ' Create a CryptoStream using the FileStream 
            ' and the passed key and initialization vector (IV).
            Dim cStream As New CryptoStream(fStream, _
                                           RijndaelAlg.CreateEncryptor(Key, IV), _
                                           CryptoStreamMode.Write)

            ' Create a StreamWriter using the CryptoStream.
            Dim sWriter As New StreamWriter(cStream)

            Try

                ' Write the data to the stream 
                ' to encrypt it.
                sWriter.WriteLine(Data)
            Catch e As Exception

                Console.WriteLine("An error occurred: {0}", e.Message)

            Finally

                ' Close the streams and
                ' close the file.
                sWriter.Close()
                cStream.Close()
                fStream.Close()

            End Try
        Catch e As CryptographicException
            Console.WriteLine("A Cryptographic error occurred: {0}", e.Message)
        Catch e As UnauthorizedAccessException
            Console.WriteLine("A file error occurred: {0}", e.Message)
        End Try
    End Sub


    Function DecryptTextFromFile(ByVal FileName As String, ByVal Key() As Byte, ByVal IV() As Byte) As String
        Try
            ' Create or open the specified file. 
            Dim fStream As FileStream = File.Open(FileName, FileMode.OpenOrCreate)
            fStream.Seek(258, SeekOrigin.Begin)

            ' Create a new Rijndael object.
            Dim RijndaelAlg As Rijndael = Rijndael.Create

            ' Create a CryptoStream using the FileStream 
            ' and the passed key and initialization vector (IV).
            Dim cStream As New CryptoStream(fStream, _
                                            RijndaelAlg.CreateDecryptor(Key, IV), _
                                            CryptoStreamMode.Read)

            ' Create a StreamReader using the CryptoStream.
            Dim sReader As New StreamReader(cStream)

            ' Read the data from the stream 
            ' to decrypt it.
            Dim val As String = Nothing

            Try
                val = sReader.ReadLine()

            Catch e As Exception
                Console.WriteLine("An Cerror occurred: {0}", e.Message)
            Finally
                ' Close the streams and
                ' close the file.
                sReader.Close()
                cStream.Close()
                fStream.Close()


            End Try

            ' Return the string. 
            Return val

        Catch e As CryptographicException
            Console.WriteLine("A Cryptographic error occurred: {0}", e.Message)
            Return Nothing
        Catch e As UnauthorizedAccessException
            Console.WriteLine("A file error occurred: {0}", e.Message)
            Return Nothing
        End Try
    End Function




End Module
