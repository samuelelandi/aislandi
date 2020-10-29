package com.aisparser;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

/** Tests for {@link Message4}. */
@RunWith(JUnit4.class)
public class Message4Test {

    @Test
    public void testParse() {
        Message4 msg = new Message4();

        try {
            Vdm vdm_message = new Vdm();
            int result = vdm_message.add("!AIVDM,1,1,,A,403OwpiuIKl:Ro=sbvK=CG700<3b,0*5E");
            assertEquals("vdm add failed", 0, result);
            msg.parse(vdm_message.sixbit());
        } catch (Exception e) {
            fail(e.getMessage());
        }

        assertEquals("msgid", 4, msg.msgid());
        assertEquals("repeat", 0, msg.repeat());
        assertEquals("userid", 3669987, msg.userid());
        assertEquals("utc_year", 2006, msg.utc_year());
        assertEquals("utc_month", 5, msg.utc_month());
        assertEquals("utc_day", 23, msg.utc_day());
        assertEquals("utc_hour", 20, msg.utc_hour());
        assertEquals("utc_minute", 10, msg.utc_minute());
        assertEquals("utc_second", 34, msg.utc_second());
        assertEquals("pos_acc", 1, msg.pos_acc());
        assertEquals("longitude", -73671329, msg.longitude());
        assertEquals("latitude", 28529500, msg.latitude());
        assertEquals("pos_type", 7, msg.pos_type());
        assertEquals("spare", 0, msg.spare());
        assertEquals("raim", 0, msg.raim());
        assertEquals("sync_state", 0, msg.sync_state());
        assertEquals("slot_timeout", 3, msg.slot_timeout());
        assertEquals("sub_message", 234, msg.sub_message());
    }
}
