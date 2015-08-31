using System;
using System.Globalization;

namespace jk.ShapefileToGrid
{
    
    /// <summary>
    /// contains some helper functions used in the menu for converting
    /// user-input numbers
    /// </summary>
    class Utils
    {
        /// <summary>
        /// This function converts a string to double-precision commas
        /// it works for both point and comma decimal separators
        /// </summary>
        /// <param name="input">the input string (should contain numeric characters)</param>
        /// <param name="result">the result number</param>
        /// <returns>true if conversion successful, false if conversion failed</returns>
        public static bool string2double(string input, out double result)
        {
            //first, try reading number using current culture specification
            if (double.TryParse(input, out result) == true)
            {
                return true;
            }
            else 
            { 
                //try reading number assuming decimal point
                CultureInfo cult = new CultureInfo("en-US");
                NumberFormatInfo decimalPointFormat = cult.NumberFormat;
                return double.TryParse(input, NumberStyles.Number, decimalPointFormat, out result);
            }
                
            ////numFormat.
            //double.TryParse(input, NumberStyles.Float, numFormat, out result);
            
            //bool ok = false;
            //int i1, i2;
            //string sep = ".,";
            //char[] separators = sep.ToCharArray();
            //if (input.Length == 0)
            //{
            //    result = 0f;
            //    return false;
            //}
            //input = input.Trim();
            //int decPointIndex = input.IndexOfAny(separators);
            //if (decPointIndex >= 0)
            //{
            //    string part1 = input.Substring(0, decPointIndex);
            //    string part2 = input.Substring(decPointIndex + 1);
            //    ok = int.TryParse(part1, out i1);
            //    ok = int.TryParse(part2, out i2);
            //    if (ok)
            //    {
            //        result = i1 + i2 / (Math.Pow(10, part2.Length));
            //        return true;
            //    }
            //    else
            //    {
            //        result = 0f;
            //        return false;
            //    }
            //}
            //else
            //{
            //    ok = double.TryParse(input, out result);
            //    if (ok)
            //    {
            //        return true;
            //    }
            //    else
            //    {
            //        return false;
            //    }
            //}
        }
    }
}
