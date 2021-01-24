//
//  4 Memories in EEPROM
//  use "platformio run -t uploadeep" after uploading the program for programing the eeprom
//
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef memory_h
#define memory_h

#include <avr/eeprom.h>

extern uint8_t EEMEM Memory1[];
extern uint8_t EEMEM Memory2[];
extern uint8_t EEMEM Memory3[];
extern uint8_t EEMEM Memory4[];
#endif
