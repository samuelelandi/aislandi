package com.aisparser;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

/** Tests for {@link Message13}. */
@RunWith(JUnit4.class)
public class Message13Test {

    @Test
    public void testParse() {
        Message13 msg = new Message13();

        try {
            Vdm vdm_message = new Vdm();
            int result = vdm_message.add("!AIVDM,1,1,,A,=03Owpi;Eo7`,0*7F");
            assertEquals("vdm add failed", 0, result);

            msg.parse(vdm_message.sixbit());
        } catch (Exception e) {
            fail(e.getMessage());
        }

        assertEquals("num_acks", 1, msg.num_acks());
        assertEquals("msgid", 13, msg.msgid());
        assertEquals("repeat", 0, msg.repeat());
        assertEquals("userid", 3669987, msg.userid());
        assertEquals("spare", 0, msg.spare());
        assertEquals("destid_1", 316005498, msg.destid_1());
        assertEquals("sequence_1", 0, msg.sequence_1());
    }
}
