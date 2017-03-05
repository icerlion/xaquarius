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
 * FileName: mbinarywriter.cs
 * 
 * Purpose:  Define binary writer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:39
===================================================================+*/
using System.IO;
using System.Net;
using System;

namespace NetIOLayer
{
    /// <summary>
    /// Convert byte order from host to net.
    /// </summary>
    public class MBinaryWriter : BinaryWriter
    {
        // Create
        public MBinaryWriter()
        {
            OutStream = new MemoryStream();
        }

        public void Clear()
        {
            OutStream.SetLength(0);
        }

        public byte[] ToArray()
        {
            return ((MemoryStream)OutStream).ToArray();
        }

        public override void Write(short value)
        {
            base.Write(IPAddress.HostToNetworkOrder(value));
        }

        public override void Write(ushort value)
        {
            Write((short)value);
        }

        public override void Write(int value)
        {
            base.Write(IPAddress.HostToNetworkOrder(value));
        }

        public override void Write(uint value)
        {
            Write((int)value);
        }

        public override void Write(long value)
        {
            base.Write(IPAddress.HostToNetworkOrder(value));
        }

        public override void Write(ulong value)
        {
            Write((long)value);
        }

        // Write string, input the fixed-length of the string
        public void WriteString(string value, ushort len)
        {
            if (value.Length >= len)
            {
                Console.WriteLine("Error! WriteString: string was too length, value:[" + value + "], expect len:[" + len + "]");
            }
            byte[] data_buff = new byte[len];
            ushort data_buff_index = 0;
            if (null != value && value.Length < len)
            {
                byte[] value_bytes = System.Text.Encoding.UTF8.GetBytes(value); ;
                for (; data_buff_index < value_bytes.Length; ++data_buff_index)
                {
                    data_buff[data_buff_index] = value_bytes[data_buff_index];
                }
            }
            for (; data_buff_index < len; ++data_buff_index)
            {
                data_buff[data_buff_index] = 0;
            }
            base.Write(data_buff);
        }

        public override void Close()
        {
            // Close
            base.Close();
            OutStream.Close();
            Dispose(true);
            OutStream.Dispose();
        }
    }
}
