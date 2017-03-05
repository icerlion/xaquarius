/*+==================================================================
 * Copyright (C) 2017 XAQUARIUS. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * FileName: mbinaryreader.cs
 * 
 * Purpose:  Define binary reader
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:39
===================================================================+*/
using System.IO;
using System.Net;
using System.Text;


namespace NetIOLayer
{
    /// <summary>
    /// Convert byte order from net to host.
    /// </summary>
    public class MBinaryReader : BinaryReader
    {
        public MBinaryReader(byte[] bytes)
            :base(new MemoryStream(bytes))
        {
        }

        public override short ReadInt16()
        {
            return IPAddress.NetworkToHostOrder(base.ReadInt16());
        }

        public override ushort ReadUInt16()
        {
            return (ushort)(ReadInt16());
        }

        public override int ReadInt32()
        {
            return IPAddress.NetworkToHostOrder(base.ReadInt32());
        }

        public override uint ReadUInt32()
        {
            return (uint)(ReadInt32());
        }

        public override long ReadInt64()
        {
            return IPAddress.NetworkToHostOrder(base.ReadInt64());
        }

        public override ulong ReadUInt64()
        {
            return (ulong)ReadInt64();
        }

        // Read the fixed-length string
        public string ReadString(ushort len)
        {
            byte[] bytes_array = base.ReadBytes((int)len);
            int str_len = 0;
            for (int i = 0; i < bytes_array.Length; ++i)
            {
                if (bytes_array[i] == '\0')
                {
                    break;
                }
                else
                {
                    ++str_len;
                }
            }
            return Encoding.UTF8.GetString(bytes_array, 0, str_len);
        }

        public override void Close()
        {
            base.Close();
            Dispose(true);
        }
    }
}
