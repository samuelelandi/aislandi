package com.aisparser;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

/** Tests for {@link Message23}. */
@RunWith(JUnit4.class)
public class Message23Test {

    @Test
    @Ignore("Need test data")
    public void testParse() {
        Message23 msg = new Message23();

        fail("Not implemented - need test data");

        try {
            Vdm vdm_message = new Vdm();
            int result = vdm_message.add("");
            assertEquals("vdm add failed", 0, result);

            msg.parse(vdm_message.sixbit());
        } catch (Exception e) {
            fail(e.getMessage());
        }

        assertEquals("msgid", 23, msg.msgid());
        assertEquals("repeat", 0, msg.repeat());
        assertEquals("userid", 0, msg.userid());
    }
}
