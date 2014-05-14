// tstore5.cpp -- Regression test program, storage tests, part 5
// $Id: tstore5.cpp,v 1.7 2003/01/09 13:54:22 wcvs Exp $
// This is part of MetaKit, see http://www.equi4.com/metakit/

#include "regress.h"

void TestStores5()
{
  B(s40, LoadFrom after commit, 0) W(s40a);
  {
    c4_IntProp p1 ("p1");

    { // create datafile by streaming out
      c4_Storage s1;
      s1.SetStructure("a[p1:I]");

      c4_View v1 = s1.View("a");
      v1.Add(p1 [123]);
	A(p1 (v1[0]) == 123);
	A(v1.GetSize() == 1);

      c4_FileStream fs1 (fopen("s40a", "wb"), true);
      s1.SaveTo(fs1);
    }
    { // it should load just fine
      c4_Storage s2;
      c4_FileStream fs1 (fopen("s40a", "rb"), true);
      bool ok = s2.LoadFrom(fs1);
      	A(ok);

      c4_View v1 = s2.View("a");
	A(p1 (v1[0]) == 123);
	A(v1.GetSize() == 1);
    }
    { // open the datafile and commit a change
      c4_Storage s3 ("s40a", true);

      c4_View v1 = s3.View("a");
	A(p1 (v1[0]) == 123);
	A(v1.GetSize() == 1);
      p1 (v1[0]) = 456;
      s3.Commit();
	A(p1 (v1[0]) == 456);
	A(v1.GetSize() == 1);
    }
    { // it should load fine and show the last changes
      c4_Storage s4;
      c4_FileStream fs1 (fopen("s40a", "rb"), true);
      bool ok = s4.LoadFrom(fs1);
      	A(ok);

      c4_View v1 = s4.View("a");
	A(p1 (v1[0]) == 456);
	A(v1.GetSize() == 1);
    }
    { // it should open just fine in the normal way as well
      c4_Storage s5 ("s40a", false);
      c4_View v1 = s5.View("a");
	A(p1 (v1[0]) == 456);
	A(v1.GetSize() == 1);
    }
  } D(s40a); R(s40a); E;

    // 2002-03-13: failure on Win32, Modify calls base class GetNthMemoCol
  B(s41, Partial modify blocked, 0) W(s41a);
  {
    c4_BytesProp p1 ("p1");
    c4_Storage s1 ("s41a", true);
    c4_View v1 = s1.GetAs("a[_B[p1:B]]");

    // custom viewers did not support partial access in 2.4.3
    c4_View v2 = v1.Blocked();
    s1.Commit();

    v2.SetSize(1);

    c4_BytesRef m = p1 (v2[0]);
    m.Modify(c4_Bytes ("abcdefgh", 8), 0);

    s1.Commit();

  } D(s41a); R(s41a); E;

  B(s42, Get descriptions, 0)
  {
    c4_Storage s1;
    s1.SetStructure("a[p1:I],b[p2:S]");
    
      c4_String x1 = s1.Description();
      A(x1 == "a[p1:I],b[p2:S]");
    
      c4_String x2 = s1.Description("b");
      A(x2 == "p2:S");
    
      const char* cp = s1.Description("c");
      A(cp == 0);
  } E;

    // 2002-04-24: VPI subview ints clobbered
  B(s43, View reuse after sub-byte ints, 0) W(s43a);
  {
    c4_IntProp p1 ("p1");
    c4_Storage s1 ("s43a", true);
    c4_View v1 = s1.GetAs("a[p1:I]");

    v1.Add(p1 [0]);
    v1.Add(p1 [1]);
    s1.Commit();

    v1.SetSize(1); // 1 is an even trickier bug than 0
    s1.Commit();

    // adding the following two lines works around the 2.4.4 bug
    //s1.Rollback();
    //v1 = s1.GetAs("a[p1:I]");
    
    v1.Add(p1 [12345]);
    s1.Commit();
    
      //int n = p1 (v1[1]);
      A(p1 (v1[1]) == 12345);

  } D(s43a); R(s43a); E;

  B(s44, Bad memo free space, 0) W(s44a);
  {
    c4_IntProp p1 ("p1");
    c4_BytesProp p2 ("p2");
    c4_Storage s1 ("s44a", true);
    c4_View v1 = s1.GetAs("a[p1:I,p2:B]");

    c4_Bytes data;
    t4_byte* p = data.SetBuffer(12345);
    for (int i = 0; i < data.Size(); ++i)
      p[i] = (t4_byte) i;

    v1.Add(p2 [data]);
    s1.Commit();

    p1 (v1[0]) = 1;
    s1.Commit();

    p1 (v1[0]) = 0;
    s1.Commit();

    c4_Bytes temp = p2 (v1[0]);
      A(temp == data); // this failed in 2.4.5

  } D(s44a); R(s44a); E;

  B(s45, Bad subview memo free space, 0) W(s45a);
  {
    c4_IntProp p1 ("p1");
    c4_ViewProp p2 ("p2");
    c4_BytesProp p3 ("p3");
    c4_Storage s1 ("s45a", true);
    c4_View v1 = s1.GetAs("a[p1:I,p2[p3:B]]");

    c4_Bytes data;
    t4_byte* p = data.SetBuffer(12345);
    for (int i = 0; i < data.Size(); ++i)
      p[i] = (t4_byte) i;

    v1.SetSize(1);
    c4_View v2 = p2 (v1[0]);
    v2.Add(p3 [data]);
    s1.Commit();

    p1 (v1[0]) = 1;
    s1.Commit();

    p1 (v1[0]) = 0;
    s1.Commit();

    c4_View v3 = p2 (v1[0]);
    c4_Bytes temp = p3 (v3[0]);
      A(temp == data); // this failed in 2.4.5

  } D(s45a); R(s45a); E;

  B(s46, LoadFrom after commit, 0) W(s46a);
  {
    c4_IntProp p1 ("p1");

    {
      c4_Storage s1 ("s46a", true);
      s1.SetStructure("a[p1:I]");
      c4_View v1 = s1.View("a");

      v1.Add(p1 [11]);
      v1.Add(p1 [22]);
      v1.Add(p1 [33]);
      v1.Add(p1 [44]);
      v1.Add(p1 [55]);
      v1.Add(p1 [66]);
      v1.Add(p1 [77]);
      v1.Add(p1 [88]);
      v1.Add(p1 [99]);
      
      s1.Commit();
    }
    {
      c4_Storage s2 ("s46a", true);
      c4_View v2 = s2.View("a");

      v2.Add(p1 [1000]); // force 1->2 byte ints
      v2.InsertAt(7, c4_Row ());
      v2.InsertAt(4, c4_Row ());

      //for (int i = 6; i <= 9; ++i) printf("%d\n", (int) p1 (v2[i]));

      A(p1 (v2[6]) == 66);
      A(p1 (v2[8]) == 0);
      A(p1 (v2[9]) == 88);
      A(p1 (v2[7]) == 77); // this failed in 2.4.6

      s2.Commit();
    }
  } D(s46a); R(s46a); E;
}
