#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Command.hpp"
#include "MessageBuffer.hpp"
#include "utils.hpp"

extern int g_signal;

static int rollDice() {
    return (std::rand() % 6) + 1;
}

static std::string getRandomFact() {
    static std::vector<std::string> facts;
    if (facts.empty()) {
        facts.push_back(
            "Earth's rotation is changing speed. It's actually slowing, and "
            "this means that, on average, the length of a day increases by "
            "around 1.8 seconds per century.");
        facts.push_back(
            "Ironically, the fear of long words is called "
            "Hippopotomonstrosesquippedaliophobia. The 36-letter word was "
            "first used in the first century BCE to criticise writers with an "
            "unreasonable penchant for long words.");
        facts.push_back(
            "Chainsaws were first invented for childbirth. They were developed "
            "in Scotland in the late 18th Century to help aid and speed up the "
            "process of symphysiotomy (widening the pubic cartilage), and the "
            "removal of disease-laden bone during childbirth.");
        facts.push_back(
            "Avocados are a fruit, not a vegetable. They're technically "
            "considered a single-seeded berry, believe it or not.");
        facts.push_back(
            "The Eiffel Tower can be 15 cm taller during the summer. That's "
            "due to thermal expansion, meaning the iron heats up, and the "
            "particles gain kinetic energy and take up more space.");
        facts.push_back(
            "Trypophobia is the fear of closely-packed holes. Or, more "
            "specifically, \"an aversion to the sight of irregular patterns or "
            "clusters of small holes or bumps.\" No English muffins for them, "
            "then!");
        facts.push_back(
            "Allodoxaphobia is the fear of other people's opinions. It's a "
            "rare social phobia characterized by an irrational and "
            "overwhelming fear of what other people think.");
        facts.push_back(
            "Australia is wider than the moon. The moon is 3400 km in "
            "diameter, while Australia’s diameter from east to west is almost "
            "4000 km.");
        facts.push_back(
            "'Mellifluous' is a sound that is pleasingly smooth and musical to "
            "hear.");
        facts.push_back(
            "The Spice Girls were originally a band called Touch. \"When we "
            "first started [with the name Touch], we were pretty bland,\" Mel "
            "C told The Guardian in 2018. \"We felt like we had to fit into a "
            "mold.\"");
        facts.push_back(
            "Human teeth are the only part of the body that cannot heal "
            "themselves. That's because teeth are coated in enamel, which is "
            "not a living tissue.");
        facts.push_back(
            "It's illegal to own just one guinea pig in Switzerland. It's "
            "considered animal abuse, because they're social beings and get "
            "lonely.");
        facts.push_back(
            "The Ancient Romans used to drop a piece of toast into their wine "
            "for good health. Hence why we \"raise a toast.\"");
        facts.push_back(
            "The heart of a shrimp is located in its head. They also have an "
            "open circulatory system, which means they have no arteries and "
            "their organs float directly in blood.");
        facts.push_back(
            "Amy Poehler was only seven years older than Rachel McAdams when "
            "she took on the role of \"cool mom\" in Mean Girls. Rachel was "
            "25, playing Regina George, and Amy was 32.");
        facts.push_back(
            "People are more creative in the shower. When we take a warm "
            "shower, we experience an increased dopamine flow that makes us "
            "more creative.");
        facts.push_back("Baby rabbits are called kits. Cute!");
        facts.push_back(
            "The unicorn is the national animal of Scotland. It was apparently "
            "chosen because of its connection with dominance and chivalry, as "
            "well as purity and innocence in Celtic mythology.");
        facts.push_back(
            "The first airplane flew on December 17, 1903. Wilbur and Orville "
            "Wright made four brief flights in Kitty Hawk, North Carolina, "
            "with their powered aircraft.");
        facts.push_back(
            "Venus is the only planet to spin clockwise. It travels around the "
            "sun once every 225 Earth days, but it rotates clockwise once "
            "every 243 days.");
        facts.push_back(
            "Nutmeg is a hallucinogen. The spice contains myristicin, a "
            "natural compound that has mind-altering effects if ingested in "
            "large doses.");
        facts.push_back(
            "A 73-year-old bottle of French Burgundy became the most expensive "
            "bottle of wine ever sold at auction in 2018, going for $558,000. "
            "The bottle of 1945 Romanee-Conti sold at Sotheby's for more than "
            "17 times its original estimate of $32,000.");
        facts.push_back(
            "Competitive art used to be an Olympic sport. Between 1912 and "
            "1948, the international sporting event awarded medals for music, "
            "painting, sculpture, and architecture.");
        facts.push_back(
            "A chef's hat has 100 pleats. Apparently, it's meant to represent "
            "the 100 ways you can cook an egg. Wonder if Gordon Ramsay knows "
            "that.");
        facts.push_back(
            "In 2014, there was a Tinder match in Antarctica. Two research "
            "scientists matched on the global dating app in the most remote "
            "part of the world. The man was working at the United States "
            "Antarctic McMurdo Station, and the woman was camping a 45-minute "
            "helicopter ride away.");
        facts.push_back(
            "The Spanish national anthem has no words. The \"Marcha Real\" is "
            "one of only four national anthems in the world (along with those "
            "of Bosnia and Herzegovina, Kosovo, and San Marino) to have no "
            "official lyrics.");
        facts.push_back(
            "The Japanese word 'Kuchi zamishi' is the act of eating when "
            "you're not hungry because your mouth is lonely. We do this all "
            "the time.");
        facts.push_back(
            "The probability of a blue lobster existing is widely touted as "
            "being one in two million. Bright blue lobsters get their color "
            "because of a genetic abnormality that causes them to produce more "
            "of a certain protein than others.");
        facts.push_back(
            "There’s only one letter that doesn’t appear in any American state "
            "name. There's a Z in Arizona, and an X in Texas, but no Q in any "
            "of them.");
        facts.push_back(
            "Louboutin's iconic red soles were inspired by Andy Warhol. The "
            "60s pop artist's drawing Flowers caught the eye of the famous "
            "designer, which gave him the idea to add the infamous sole to his "
            "designs.");
        facts.push_back(
            "A book called \"A La Recherche du Temps Perdu\" by Marcel Proust "
            "contains an estimated 9,609,000 characters, making it the longest "
            "book in the world. The title translates to \"Remembrance of "
            "Things Past.\"");
        facts.push_back(
            "Google Images was created after Jennifer Lopez wore *that* "
            "infamous dress at the 2000 Grammys. So many people were searching "
            "for her outfit, the search engine added an image function.");
        facts.push_back(
            "Big Ben's clock stopped at 10:07 p.m. on 27 May 2005, most likely "
            "due to extremely hot temperatures.");
        facts.push_back(
            "Walt Disney currently holds the most Academy Awards. Disney won "
            "26 Oscars over the course of his career and was nominated 59 "
            "times.");
        facts.push_back(
            "There's a fruit that tastes like chocolate pudding. It's called "
            "black sapote, and it's native to Central and South America. "
            "Apparently it tastes like chocolate and sweet custard.");
        facts.push_back(
            "Queen Elizabeth II was a trained mechanic. When she was 16, the "
            "Queen joined the British employment agency at the Labour Exchange "
            "and learned the basics of truck repair.");
        facts.push_back(
            "The Easter Island heads have bodies. In the 2010s, archaeologists "
            "found that two of the Pacific Island figures actually had torsos "
            "that measured as high as 33 feet.");
        facts.push_back(
            "M&Ms are named after the businessmen who created them. But what "
            "do the Ms stand for? Forrest Mars and Bruce Murrie.");
        facts.push_back(
            "Pigeons can tell the difference between Picasso and Monet. What?! "
            "A 1995 study shows that the birds can differentiate between the "
            "two artists.");
        facts.push_back(
            "The real name for a hashtag is an octothorpe. The 'octo' refers "
            "to the eight points in the symbol, but according to the "
            "Merriam-Webster dictionary, the 'thorpe' part is a mystery.");
        facts.push_back(
            "The actors who voiced Mickey and Minnie Mouse got married in real "
            "life. Russi Taylor (Minnie) and Wayne Allwine (Mickey) tied the "
            "knot in 1991.");
        facts.push_back(
            "You can hear a blue whale's heartbeat from over two miles away. "
            "Blue whales weigh an average of between 130,000 and 150,000 kg, "
            "with their hearts weighing roughly 180 kg.");
        facts.push_back(
            "The last letter added to the English alphabet was 'J'. The letter "
            "dates back to 1524, and before that the letter 'i' was used for "
            "both the 'i' and 'j' sounds.");
        facts.push_back(
            "There is actually a word for someone giving an opinion about "
            "something they know nothing about. An 'ultracrepidarian' is "
            "someone who voices thoughts beyond their expertise.");
        facts.push_back(
            "The Chupa Chups logo was designed by Salvador Dalí. The "
            "surrealist artist designed the logo in 1969.");
        facts.push_back(
            "Ketchup was once sold as medicine. The condiment was prescribed "
            "and sold to people suffering from indigestion back in 1834.");
        facts.push_back(
            "The world's longest walking distance is 14,000 miles. You can "
            "walk from Magadan in Russia to Cape Town in South Africa. It "
            "requires no flying or sailing—just bridges and open roads.");
        facts.push_back(
            "The moon has moonquakes. They happen due to tidal stresses "
            "connected to the distance between the moon and the Earth.");
        facts.push_back(
            "Humans are the only animals that blush. Apparently, we're also "
            "the only animals that experience embarrassment, too. This is "
            "because it's a complex emotion that involves understanding other "
            "people's opinions.");
        facts.push_back(
            "All the clocks in Pulp Fiction are set to 4:20. Looks like we're "
            "going to have to rewatch the film to prove it.");
        facts.push_back(
            "Kim Kardashian has a 'glam' clause in her will. It states that if "
            "she's ever in a position where she can't get ready herself, can't "
            "communicate, or is unconscious, someone has to make sure her "
            "hair, nails, and makeup are all perfect.");
        facts.push_back(
            "An ostrich's eye is bigger than its brain. This could be why the "
            "birds often run around in circles when trying to escape "
            "predators.");
        facts.push_back(
            "A jiffy is an actual unit of time. It's 1/100th of a second.");
        facts.push_back(
            "You can't hum if you hold your nose. Hands up if you just tried "
            "it!");
        facts.push_back(
            "Vatican City is the smallest country in the world. It's 120 times "
            "smaller than the island of Manhattan.");
        facts.push_back(
            "Japan has over 200 flavors of Kit Kats. They're exclusively "
            "created for different regions, cities, and seasons. There are "
            "some tasty-sounding ones like banana, blueberry cheesecake, and "
            "Oreo ice cream, as well as some very questionable ones like baked "
            "potato, melon and cheese, wasabi, and vegetable juice.");
        facts.push_back(
            "Kris Jenner has 13 grandchildren. (Extra points if you can list "
            "them in order of age.) Kourtney Kardashian has four children, Kim "
            "has four, Rob has one, Khloé has two, and Kylie has two.");
        facts.push_back(
            "A crocodile cannot stick its tongue out. They have a membrane "
            "that holds their tongue in place on the roof of their mouth, so "
            "it can't move.");
        facts.push_back(
            "Pigs can't look up into the sky. The anatomy of their spine and "
            "neck muscles limits their movement and restricts their head from "
            "being able to look upwards.");
        facts.push_back(
            "Kim Kardashian knows the alphabet in sign language. Apparently, "
            "she used it to cheat on tests with her friends in school.");
        facts.push_back(
            "In the course of an average lifetime, while sleeping you may eat "
            "around 70 different insects and 10 spiders, or more. Ew.");
        facts.push_back(
            "Everyone's tongue print is different. Like the fingerprint, it "
            "has unique features that differ from person to person.");
        facts.push_back(
            "It would take 19 minutes to fall to the center of the Earth. "
            "Scientists have worked this out, but they haven't tried it in "
            "real life.");
        facts.push_back(
            "The Night's Watch cloaks in Game of Thrones were made from Ikea "
            "rugs. The show bought the rugs, shaved them down, and then dyed "
            "them to make the cloaks.");
        facts.push_back(
            "The iCarly high school set was also used for Saved by the Bell "
            "and That's So Raven. That's why it looks so familiar!");
        facts.push_back(
            "Harry Styles has four nipples. He has a common condition called "
            "polythelia.");
        facts.push_back(
            "People used to say \"prunes\" instead of \"cheese\" when getting "
            "their pictures taken. In the 1840s, a cheesy grin was seen as "
            "childish, so a London photographer told people to say \"prunes\" "
            "to keep their mouths tight.");
        facts.push_back(
            "Blue whale tongues can weigh as much as an elephant. Their hearts "
            "also only need to beat once every ten seconds.");
        facts.push_back(
            "Queen Elizabeth II had a stand-in to make sure the sun wouldn't "
            "get in her eyes. Ella Slack was a similar height to the late "
            "Queen, so she would rehearse big events beforehand to make sure "
            "everything would be comfortable for the monarch. However, she "
            "wasn't allowed to sit on the throne, so she had to squat above "
            "it.");
        facts.push_back(
            "There's an ant species unique to New York City. Called "
            "ManhattAnts, biologists found them in a specific 14-block strip "
            "of the city.");
        facts.push_back(
            "The Eiffel Tower was originally intended for Barcelona. The "
            "Spanish city thought it was too ugly, so Gustave Eiffel pitched "
            "it to Paris instead. French critics weren't too thrilled either, "
            "but it's one of the most famous landmarks in the world today.");
        facts.push_back(
            "There's only one Shell garage actually shaped like a Shell. There "
            "was actually eight built in the 1930s, but only one remains, in "
            "North Carolina.");
        facts.push_back(
            "The shortest commercial flight in the world takes place in "
            "Scotland. Regional airline Loganair flies between the islands "
            "Westray and Papa Westray. The journey is just 1.7 miles and takes "
            "90 seconds.");
        facts.push_back(
            "Dolphins have names for one another. According to National "
            "Geographic, dolphins use a unique whistle to distinguish between "
            "different members in their pod.");
        facts.push_back(
            "The blob of toothpaste on a toothbrush has a name. It's called a "
            "\"nurdle\" and there was once a lawsuit over which toothpaste "
            "company had the right to portray it.");
        facts.push_back(
            "One part of Istanbul is in Europe, and the other is in Asia. Part "
            "of it neighbors Greece and Bulgaria (therefore sitting in "
            "Europe), and the other part neighbors Syria, Iran, and Iraq, "
            "beyond Turkey’s borders (therefore classed as Asia). The "
            "Bosphorus Strait runs between them—a narrow body of water that "
            "connects the Black Sea to the Mediterranean Sea via the Sea of "
            "Marmara.");
        facts.push_back(
            "There are more than 1,000 types of bananas growing in the world. "
            "These can then be divided into 50 categories.");
        facts.push_back(
            "The letter 'E' is the most popular letter in the alphabet. 'Z' is "
            "the least popular.");
        facts.push_back(
            "Hazelnuts are the official state nut of Oregon. Because yes, "
            "that's a thing.");
        facts.push_back(
            "The Beatles' song \"A Day in the Life\" has a frequency only dogs "
            "can hear. It's at the end of the song.");
        facts.push_back(
            "Vietnam is the world's second biggest coffee producer. Brazil "
            "creates the most, with Colombia coming in third.");
        facts.push_back(
            "The TV show Big Brother originated in the Netherlands. It was "
            "created by John de Mol in 1999, and has since been franchised in "
            "54 countries.");
        facts.push_back(
            "Candles were once eaten in famines. They used to be mainly "
            "created from beef fat or beeswax, and so they were often stolen "
            "in famines.");
        facts.push_back(
            "KFC is the meal of choice on Christmas Day in Japan. This "
            "happened following a successful marketing campaign over 40 years "
            "ago.");
        facts.push_back(
            "The bamboo plant is the fastest-growing woody plant in the world. "
            "It can grow up to 35 inches per day.");
        facts.push_back(
            "The office chair is believed to have been invented by Charles "
            "Darwin. The scientist had the idea to attach wheels to his chair "
            "so he could move around between his many tables.");
        facts.push_back(
            "Mariah Carey holds the record for the most number-one singles by "
            "a solo artist on the Billboard Hot 100 chart. She surpassed Elvis "
            "Presley and Michael Jackson with singles including \"Fantasy\" "
            "and \"Always Be My Baby.\"");
        facts.push_back(
            "Only 10% of the population is left-handed. And men are more "
            "likely to be left-handed.");
        facts.push_back(
            "A cat spends on average 15 hours a day sleeping (jealous). This "
            "means the majority of its life is spent asleep.");
        facts.push_back(
            "You share your birthday with around 19 million people. Yep, not "
            "feeling so special now, are you?");
        facts.push_back(
            "There are about 5,000 species of sea sponges. Really!");
        facts.push_back(
            "Beyoncé is the most awarded artist at the Grammys. She's been "
            "nominated for 88 awards and won a whopping 32.");
        facts.push_back(
            "The word \"cereal\" comes from a Roman goddess. The breakfast "
            "food is named after the goddess of harvest and agriculture, "
            "Ceres.");
        facts.push_back(
            "The tiny pocket in your jeans was designed to hold pocket "
            "watches. It was needed for cowboys in the 1800s, although now "
            "it's mainly a decorative feature.");
        facts.push_back(
            "The Pope can't be an organ donor. All popes' bodies belong to the "
            "Vatican when they die, which means no organ donation is allowed.");
        facts.push_back(
            "Lemons float, but limes sink. This is because of their different "
            "densities, often causing unpeeled limes to sink if submerged in a "
            "liquid.");
        facts.push_back(
            "The continent of Africa is in all four hemispheres. Brain "
            "exploding emoji!");
        facts.push_back(
            "In the TV show Gilmore Girls, 26-year-old Keiko Agena portrayed a "
            "15-year-old Lane Kim. And yet her co-star, Alexis Bledel, was 19 "
            "while playing 15-year-old Rory Gilmore.");
        facts.push_back(
            "People once ate arsenic to improve their skin. In the late 19th "
            "century, it was said to get rid of freckles and blackheads.");
        facts.push_back(
            "Julius Caesar was stabbed 23 times when he died. The Roman leader "
            "was killed by his fellow senators, including his former bestie, "
            "Brutus.");
        facts.push_back(
            "After water, tea is the most consumed drink in the world. Pinkies "
            "up!");
        facts.push_back(
            "During the 2024 Summer Olympics, four sports made their debut. "
            "Climbing, breaking, skateboarding, and surfing.");
        facts.push_back(
            "Onions are a natural remedy for clearing a blocked nose. The "
            "sulfur content in the vegetable is thought to draw mucus out of "
            "the body.");
        facts.push_back(
            "Nepal has the most public holidays. The nation has 39 each year.");
        facts.push_back(
            "The longest bone in the human body is called the femur. It's your "
            "thigh bone.");
        facts.push_back(
            "In order for a spirit to be classed as tequila, it needs to be "
            "made from the blue agave plant. In fact, it's made from the "
            "center of the agave plant, which is called the piña.");
        facts.push_back(
            "Actor Julia Roberts is aunt to fellow actor Emma Roberts. Also an "
            "actor, Eric Roberts is Julia's older brother and Emma's father.");
        facts.push_back(
            "Sheffield FC is the oldest soccer club recognised by FIFA, having "
            "been founded in October 1857.");
        facts.push_back(
            "Each zodiac sign belongs to an element. Fire signs are Aries, "
            "Leo, Sagittarius. Water signs are Cancer, Scorpio, and Pisces. "
            "Air signs are Gemini, Libra, and Aquarius. Earth signs are "
            "Taurus, Virgo, and Capricorn.");
        facts.push_back(
            "Raphael Nadal holds the title for the most Grand Slams ever won "
            "on clay. He won the French Open 14 times.");
        facts.push_back(
            "A plant or animal cell that has a membrane and a nucleus is "
            "called a eukaryote. All animals, plants, and fungi fall into this "
            "category.");
        facts.push_back(
            "The Caesar salad recently turned 100 years old. It was invented "
            "in July 1924 by Caesar Cardini, an Italian chef who owned a "
            "restaurant in Tijuana, Mexico.");
        facts.push_back(
            "The hottest temperature ever recorded on Earth was 134 F. The "
            "recording was taken in Death Valley, Nevada, in July 1913.");
        facts.push_back(
            "The \"Baby Shark Dance\" is one of the most-watched videos on "
            "YouTube, with over 14.66 billion views.");
        facts.push_back(
            "Prince William and Prince Harry have a half-sister named Laura, "
            "who largely stays out of the public eye. She was born in 1978, "
            "and her parents are Queen Camilla and her then-husband, Andrew "
            "Parker Bowles.");
        facts.push_back(
            "Billie Eilish's \"No Time to Die\" is the most awarded song in "
            "history. It won 36 awards, including the Grammy Award for Best "
            "Song Written for Visual Media, the Golden Globe Award for Best "
            "Original Song, the Critics' Choice Movie Award for Best Song, and "
            "the Academy Award for Best Original Song.");
        facts.push_back(
            "Pantone named Mocha Mousse their Color of the Year for 2025.");
        facts.push_back(
            "Your tongue is covered with roughly 8,000 taste buds, each "
            "containing up to 100 cells, which are what help you to taste "
            "food.");
        facts.push_back(
            "The oldest verified man to ever live is Jiroemon Kimura "
            "(1897–2013) of Japan. He lived to the age of 116 years and 54 "
            "days.");
        facts.push_back(
            "The largest wave ever recorded was in Lituya Bay in Alaska. It "
            "reached 1,720 feet in 1958, and was triggered by a massive "
            "landslide.");
        facts.push_back(
            "Coconuts are actually a fruit, not a nut. Sometimes they're also "
            "called drupes, which are defined as fruits that have an inner "
            "flesh and seed surrounded by a hard shell.");
        facts.push_back(
            "90% of the world's population lives in the Northern Hemisphere.");
        facts.push_back(
            "The largest bowling alley in the world is in Japan. It has 116 "
            "lanes.");
        facts.push_back(
            "Sudan has more pyramids than any country in the world. The "
            "country has between 200 and 250 pyramids, compared to Egypt's "
            "138.");
        facts.push_back("Japan has one vending machine for every 40 people.");
        facts.push_back(
            "The shortest war in history, the Anglo-Zanzibar War, lasted 38 "
            "minutes.");
        facts.push_back(
            "Finland has ranked as the happiest country in the world for seven "
            "years straight.");
        facts.push_back(
            "Mirrors facing each other don't produce infinite reflections. In "
            "fact, each reflection will be darker than the last and eventually "
            "fade into invisibility.");
        facts.push_back(
            "All clown fish are born male. They can become female later in "
            "life.");
        facts.push_back(
            "A rainbow on Venus is called a glory. They appear as a series of "
            "colored concentric rings.");
        facts.push_back(
            "A lightning bolt is five times hotter than the surface of the "
            "Sun.");
        facts.push_back("In Japan, you can buy square watermelons.");
        facts.push_back(
            "No number before 1,000 contains the letter 'A' when spelled out.");
        facts.push_back(
            "Ant-eaters have no teeth. Instead, they use their tongue to eat "
            "food.");
        facts.push_back(
            "The human nose can detect a trillion different scents.");
        facts.push_back(
            "White is not actually a color. Rather, it's the absence of all "
            "colors.");
        facts.push_back(
            "Your skin is the largest body organ. It's followed by your liver, "
            "brain, and lungs.");
        facts.push_back(
            "There are eight main blood types, but the rarest blood type is "
            "AB.");
        facts.push_back(
            "The Yarlung Tsangpo Grand Canyon in China is regarded as the "
            "deepest canyon in the world. It's 18,000 feet deep and is longer "
            "than the Grand Canyon.");
        facts.push_back(
            "Wind is actually silent. What we hear is the wind blowing against "
            "an object.");
        facts.push_back("One season on Saturn lasts about seven Earth years.");
        facts.push_back(
            "The Atlantic Ocean is saltier than the Pacific Ocean.");
        facts.push_back(
            "If you get a group of 23 people together, there's a 50% chance "
            "two people will have the same birthday. In a room of 75, there's "
            "a 99.9% chance at least two people will have the same birthday.");
        facts.push_back(
            "Cows are able to walk up stairs but aren't able to go down them "
            "without help.");
        facts.push_back(
            "According to data from 2023, it's thought that Muhammed is the "
            "most popular name in the world. Maria is the second most popular "
            "name.");
        facts.push_back(
            "Leap years happen every four years. They occur when the calendar "
            "contains an extra day—February 29th. The next leap year will take "
            "place in 2028.");
        facts.push_back(
            "A dog that is a cross between a Chihuahua and a miniature "
            "dachshund is called a chiweenie.");
        facts.push_back(
            "If you Google the word \"askew,\" the entire page will tilt. "
            "Warning: it might make you feel sick.");
        facts.push_back(
            "Snow White was the first Disney Princess to have a full-length "
            "movie. It was created way back in 1938.");
        facts.push_back(
            "The name LEGO comes from the Danish words “Leg Godt,” which means "
            "\"play well.\"");
        facts.push_back("A group of frogs is called an army.");
        facts.push_back(
            "The number four (4) is the only number with the same number of "
            "letters as its value. All of the others have more or less letters "
            "than its meaning.");
        facts.push_back("It’s very difficult to sneeze with your eyes open.");
        facts.push_back(
            "Human teeth are the hardest part of the body. Even more so than "
            "human bone.");
        facts.push_back(
            "Green is the rarest eye color of the main four eye color groups: "
            "blue, brown, hazel, and green.");
        facts.push_back(
            "The Pacific is the largest and deepest ocean, covering more than "
            "one-third of the Earth’s entire surface.");
        facts.push_back(
            "The average length of time for a lion to live in the wild is "
            "between 12 and 16 years. In captivity, they can live longer, up "
            "to 25 years.");
        facts.push_back(
            "The world's hottest chilli pepper is the Carolina Reaper, "
            "according to the Guinness Book of World Records. It's around 400 "
            "times spicier than a jalapeño.");
        facts.push_back(
            "Starfish don't have brains. The animal has a very simple nervous "
            "system, which doesn't include the organ.");
        facts.push_back(
            "Panda bears spend the majority of their waking hours eating. They "
            "have been known to eat for 12 hours straight.");
        facts.push_back(
            "There are two main shapes of popcorn: butterfly and mushroom. "
            "Mushroom is when the kernel pops in a more uniform way, while "
            "butterfly is more explosive.");
        facts.push_back(
            "The tallest giraffe in the world is called Forest, and he lives "
            "at the Australia Zoo in Queensland, Australia. He is 18 feet, 8 "
            "inches tall.");
        facts.push_back(
            "Your nose and ears never stop growing, so they keep getting "
            "bigger from birth. Your feet and hands, on the other hand, stop.");
        facts.push_back("Birds are descendants of dinosaurs.");
        facts.push_back("Potatoes were the first food to be grown in space.");
        facts.push_back(
            "In your lifetime, you can walk enough to circle the globe three "
            "times.");
        facts.push_back(
            "Rats have the ability to laugh. They do so when tickled.");
    }

    int i = std::rand() % facts.size();

    return facts[i];
}

class Bot {
   public:
    Bot(const int port, const std::string &password, const std::string &nick,
        const std::string &name)
        : _nick(nick), _buffer(0) {
        _socket = socket(AF_INET, SOCK_STREAM, 0);
        if (_socket < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        _buffer.setFd(_socket);

        sockaddr_in addr;
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(port);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        int res = connect(_socket, (const struct sockaddr *)&addr,
                          sizeof(sockaddr_in));
        if (res < 0) {
            std::ostringstream msg;
            msg << "Failed to connect to IRC server on 127.0.0.1:" << port
                << std::endl;
            throw std::runtime_error(msg.str());
        }

        if (!try_login(password, nick, name)) {
            throw std::runtime_error(
                "Failed to login with provided password and NICK");

            close(_socket);
        }

        if (!join_channel()) {
            throw std::runtime_error("Failed to join channel &" + _nick);

            close(_socket);
        }
    }

    ~Bot() { close(_socket); }

    void process() {
        while (wait_can_read() && !g_signal) {
            size_t bytes_received = _buffer.readFd();
            if (bytes_received <= 0) {
                std::vector<std::string> args;
                send_message("QUIT :Connection reset by peer");

                return;
            }

            while (true) {
                try {
                    std::string msg = _buffer.nextCommand();
                    Command     cmd(msg);

                    if (shouldDisconnect(cmd.getComponents())) {
                        std::cout << "Server requested disconnect" << std::endl;
                        return;
                    }

                    std::string command = getCommand(cmd.getComponents());

                    std::ostringstream reply;
                    if (command == "roll" || command == "dice") {
                        reply << "PRIVMSG &" << _nick
                              << " :The dice was rolled and it fell on: "
                              << rollDice();
                    } else if (command == "fact") {
                        reply << "PRIVMSG &" << _nick << " :"
                              << getRandomFact();

                    } else if (command.find("cat") != std::string::npos) {
                        reply << "PRIVMSG &" << _nick << " :meow";
                    }

                    if (reply.str().empty()) continue;
                    send_message(reply.str());

                    continue;
                } catch (MessageBuffer::NoCommandException &) {
                    break;
                    ;
                } catch (MessageBuffer::MalformedCommandException &) {
                    send_message("QUIT :Excess flood");

                    return;
                } catch (...) {
                    throw;
                }
            }
        }
    }

    std::string getCommand(const std::vector<std::string> &components) {
        if (components.size() == 4 && components[1] == "PRIVMSG") {
            if (components[2] == "&" + _nick) {
                return get_trailing_value(components[3]);
            }
        }

        return "";
    }

    bool shouldDisconnect(const std::vector<std::string> &components) {
        if (components.size() == 5 && components[1] == "KICK" &&
            components[2] == "&" + _nick && components[3] == _nick)
            return true;

        if (components.size() >= 1 && components[0] == "ERROR") return true;

        return false;
    }

   private:
    int           _socket;
    std::string   _nick;
    MessageBuffer _buffer;

    Bot(const Bot &target);
    Bot &operator=(const Bot &rhs);

    ssize_t send_message(std::string msg) {
        msg += "\r\n";
        return send(_socket, msg.c_str(), msg.length(), MSG_NOSIGNAL);
    }

    bool wait_can_send() {
        fd_set writefds;

        FD_ZERO(&writefds);
        FD_SET(_socket, &writefds);

        int res = select(_socket + 1, NULL, &writefds, NULL, 0);
        if (res < 0) return false;

        return FD_ISSET(_socket, &writefds);
    }

    bool wait_can_read() {
        fd_set readfds;
        fd_set failfds;

        FD_ZERO(&readfds);
        FD_SET(_socket, &readfds);
        FD_ZERO(&failfds);
        FD_SET(_socket, &failfds);

        int res = select(_socket + 1, &readfds, NULL, &failfds, 0);
        if (res < 0) return false;

        if (FD_ISSET(_socket, &failfds)) return false;

        return FD_ISSET(_socket, &readfds);
    }

    bool try_login(const std::string &password, const std::string &nick,
                   const std::string &name) {
        int         res;
        std::string message;

        message = "PASS " + password;
        res     = send_message(message);
        if (res < 0) return false;

        message = "NICK " + nick;
        res     = send_message(message);
        if (res < 0) return false;

        message = "USER " + nick + " * 0 :" + name;
        res     = send_message(message);
        if (res < 0) return false;

        while (wait_can_read() && !g_signal) {
            size_t bytes_received = _buffer.readFd();
            if (bytes_received <= 0) {
                std::vector<std::string> args;
                send_message("QUIT :Connection reset by peer");

                return false;
            }

            while (true) {
                try {
                    std::string msg = _buffer.nextCommand();
                    Command     cmd(msg);
                    if (cmd.getComponents().size() != 4) {
                        return false;
                    }

                    if (cmd.getComponents()[1] != "001") {
                        return false;
                    }

                    return true;
                } catch (MessageBuffer::NoCommandException &) {
                    break;
                } catch (MessageBuffer::MalformedCommandException &) {
                    send_message("QUIT :Excess flood");

                    return false;
                } catch (...) {
                    throw;
                }
            }
        }

        return false;
    }

    bool join_channel() {
        int         res;
        std::string message;

        message = "JOIN &" + _nick;
        res     = send_message(message);
        if (res < 0) return false;

        while (wait_can_read() && !g_signal) {
            size_t bytes_received = _buffer.readFd();
            if (bytes_received <= 0) {
                std::vector<std::string> args;
                send_message("QUIT :Connection reset by peer");

                return false;
            }

            while (true) {
                try {
                    std::string msg = _buffer.nextCommand();
                    Command     cmd(msg);

                    if (cmd.getComponents().size() != 3) {
                        return false;
                    }

                    if (cmd.getComponents()[1] != "JOIN") {
                        return false;
                    }

                    if (cmd.getComponents()[2] != "&" + _nick) {
                        return false;
                    }

                    return true;
                } catch (MessageBuffer::NoCommandException &) {
                    break;
                } catch (MessageBuffer::MalformedCommandException &) {
                    send_message("QUIT :Excess flood");

                    return false;
                } catch (...) {
                    throw;
                }
            }
        }

        return false;
    }
};

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server-port> <server-password>"
                  << std::endl;
        return 1;
    }

    std::srand(time(0));

    int port = parse_port(argv[1]);
    if (port == -1) {
        std::cerr << "Invalid port: " << argv[1] << std::endl;
        return 1;
    }

    try {
        signal(SIGINT, sigHandler);
        signal(SIGQUIT, sigHandler);

        Bot bot(port, argv[2], "ircbot", "IRC Bot");
        bot.process();
    } catch (std::exception &e) {
        std::cerr << "FATAL: " << e.what() << std::endl;
        return 1;
    }
}
