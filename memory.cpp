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


#include "memory.h"
#include <avr/eeprom.h>

uint8_t EEMEM Memory1[] = "HB9|C|H|M";
uint8_t EEMEM Memory2[] = "CQ CQ CQ DX DE HB9CHM HB9CHM HB9CHM PSE K";
uint8_t EEMEM Memory3[] = "5NN";
uint8_t EEMEM Memory4[] = "CQ CQ CQ DE HB9CHM HB9CHM HB9CHM PSE K";



