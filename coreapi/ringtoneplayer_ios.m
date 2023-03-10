/*
 * Copyright (C) 2000  Simon MORLAT (simon.morlat@linphone.org)
 * Copyright (c) 2010-2022 Belledonne Communications SARL.
 *
 * This file is part of Liblinphone 
 * (see https://gitlab.linphone.org/BC/public/liblinphone).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ringtoneplayer_ios.h"

#import <AVFoundation/AVFoundation.h>

@interface AudioPlayerDelegate : NSObject <AVAudioPlayerDelegate>
@property (assign) LinphoneRingtonePlayer* ringtonePlayer;
@end

struct _LinphoneRingtonePlayer {
	AVAudioPlayer* player;
	AudioPlayerDelegate* playerDelegate;
	LinphoneRingtonePlayerFunc end_of_ringtone;
	void* end_of_ringtone_ud;
};

@implementation AudioPlayerDelegate
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag {
	if (_ringtonePlayer->end_of_ringtone) {
		_ringtonePlayer->end_of_ringtone(_ringtonePlayer, _ringtonePlayer->end_of_ringtone_ud, !flag);
	}
}
- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error {
	if (_ringtonePlayer->end_of_ringtone) {
		_ringtonePlayer->end_of_ringtone(_ringtonePlayer, _ringtonePlayer->end_of_ringtone_ud, 1);
	}
}
@end



LinphoneRingtonePlayer* linphone_ringtoneplayer_ios_new() {
	LinphoneRingtonePlayer* rp = ms_new0(LinphoneRingtonePlayer, 1);
	rp->playerDelegate = [[AudioPlayerDelegate alloc] init];
	rp->playerDelegate.ringtonePlayer = rp;
	return rp;
}

void linphone_ringtoneplayer_ios_destroy(LinphoneRingtonePlayer* rp) {
	linphone_ringtoneplayer_ios_stop(rp);
	[rp->playerDelegate release];
	rp->playerDelegate = nil;
	ms_free(rp);
}

int linphone_ringtoneplayer_ios_start_with_cb(LinphoneRingtonePlayer* rp, const char* ringtone, int loop_pause_ms, LinphoneRingtonePlayerFunc end_of_ringtone, void * user_data) {
	NSURL* url = [NSURL URLWithString:[NSString stringWithUTF8String:ringtone]];
	ms_message("%s: using ringtone %s", __FUNCTION__, ringtone);
	if (rp->player) {
		ms_warning("%s: a player is already instantiated, stopping it first.", __FUNCTION__);
		linphone_ringtoneplayer_ios_stop(rp);
	}
	NSError * error = NULL;
	rp->player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
	if(rp->player == NULL && error != NULL) {
		ms_error("Failed in creating AVAudioPlayer: %s", error.localizedDescription.UTF8String);
		return 1;
	}

	[rp->player prepareToPlay];
	rp->player.numberOfLoops = (loop_pause_ms >= 0) ? -1 : 0;
	rp->player.delegate = rp->playerDelegate;
	rp->end_of_ringtone = end_of_ringtone;
	rp->end_of_ringtone_ud = user_data;
	return [rp->player play] ? 0 : 1;
}

bool_t linphone_ringtoneplayer_ios_is_started(LinphoneRingtonePlayer* rp) {
	return (bool_t)[rp->player isPlaying];
}

int linphone_ringtoneplayer_ios_stop(LinphoneRingtonePlayer* rp) {
	[rp->player stop];
	[rp->player release];
	rp->player = nil;
	return 0;
}
